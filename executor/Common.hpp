#pragma once
#include "WorkSimulator.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>
struct ExecutionResult {
  std::chrono::high_resolution_clock::time_point time_point;
  std::string data;
};

struct PrintableResult {
  static std::string column_names() {
    std::stringstream ss;
    ss << "Time[ns]\tThreadID\tIteration\tIndex\tRandom Sum";
    return ss.str();
  }
  ExecutionResult execution_result;
  std::chrono::high_resolution_clock::time_point start_time_point;

  friend std::ostream &operator<<(std::ostream &os,
                                  const PrintableResult &exec);
};
std::ostream &operator<<(std::ostream &os, const PrintableResult &exec) {
  os << std::chrono::duration_cast<std::chrono::nanoseconds>(
            exec.execution_result.time_point - exec.start_time_point)
            .count()
     << exec.execution_result.data;
  return os;
}
template <typename Result> struct Executable {
  std::promise<Result> promise;
  template <typename... Args> void operator()(Args &&... args) {
    promise.set_value(Result{std::forward<Args>(args)...});
  }
};
template <> struct Executable<ExecutionResult> {
  long long index;
  std::promise<ExecutionResult> promise;
  void operator()(long long counter) {
    std::stringstream ss;
    ss << '\t' << std::hex << std::this_thread::get_id() << std::dec;
    ss << '\t' << counter;
    ss << '\t' << index;
    ss << '\t' << WorkSimulator{}.work<short>();
    promise.set_value(
        ExecutionResult{std::chrono::high_resolution_clock::now(), ss.str()});
  }
};

using CountingExecutable = Executable<ExecutionResult>;

template <typename DerivedExecutor> struct BaseExecutor {
  void operator()() { static_cast<DerivedExecutor &> (*this)(); }
  void emplace(CountingExecutable *new_work) {
    static_cast<DerivedExecutor &>(*this).emplace(new_work);
  }
  void stop() { static_cast<DerivedExecutor &>(*this).stop(); }
  virtual ~BaseExecutor() = default;
};

template <typename Executor> struct CountBenchmark {
  BaseExecutor<Executor> &executor;
  std::mutex &cout_mutex;
  std::ostream &ostream;
  void operator()(int num_dispatches = 2048) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_dispatches; ++i) {
      CountingExecutable executable{i};
      auto future = executable.promise.get_future();
      executor.emplace(&executable);
      auto res = future.get();
      // Print results under mutex
      std::lock_guard guard{cout_mutex};
      ostream << PrintableResult{res, start} << std::endl;
    }
  }
};
template <typename Executor, typename Duration> struct DurationBenchmark {
  using Clock = std::chrono::high_resolution_clock;

  BaseExecutor<Executor> &executor;
  std::mutex &cout_mutex;
  std::ostream &ostream;
  Duration duration;
  bool elapsed(const Clock::time_point &start_time) {
    return Clock::now() < start_time + duration;
  }
  void operator()() {
    auto start = Clock::now();
    for (int i = 0; elapsed(start); ++i) {
      CountingExecutable executable{i};
      auto future = executable.promise.get_future();
      executor.emplace(&executable);
      auto res = future.get();
      // Print results under mutex
      std::lock_guard guard{cout_mutex};
      ostream << PrintableResult{res, start} << std::endl;
    }
  }
};

template <typename ExecutorImplementation> struct BenchmarkRunner {
  std::unique_ptr<BaseExecutor<ExecutorImplementation>> executor;
  std::mutex cout_mutex{};
  std::thread executor_thread{[&executor = *executor] { executor(); }};
  ~BenchmarkRunner() {
    executor->stop();
    executor_thread.join();
  }
  void operator()(int count) {
    std::cout << PrintableResult::column_names() << std::endl;
    {
      using namespace std::chrono_literals;
      auto num_threads = std::thread::hardware_concurrency() - 1;
      auto work_per_thread = std::div(count, num_threads);
      auto rem = work_per_thread.rem;
      std::vector<std::thread> threads(num_threads);
      for (auto &th : threads)
        th = std::thread(CountBenchmark<ExecutorImplementation>{*executor,
                                                                cout_mutex,
                                                                std::cout},
                         work_per_thread.quot +( (--rem>=0) ? 1 : 0));
      for (auto &th : threads)
        th.join();
    }
  }
  void operator()(std::chrono::seconds sec) {
    std::cout << PrintableResult::column_names() << std::endl;
    {
      using namespace std::chrono_literals;
      std::vector<std::thread> threads(std::thread::hardware_concurrency() - 1);
      for (auto &th : threads)
        th = std::thread(
            DurationBenchmark<ExecutorImplementation, std::chrono::nanoseconds>{
                *executor, cout_mutex, std::cout, sec});
      for (auto &th : threads)
        th.join();
    }
  }
};

template <typename ExecuterImplementation> struct CmdlineParser {
  int operator()(int &argc, char **&argv) {
    namespace po = boost::program_options;
    po::options_description desc("Benchmark options");
    desc.add_options()("help", "produce help message")(
        "timer", po::value<int>(),
        "run time in seconds")("count", po::value<int>(), "iterations count");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return EXIT_SUCCESS;
    }
    BenchmarkRunner<ExecuterImplementation> benchmark_runner{
        std::make_unique<ExecuterImplementation>()};
    if (vm.count("timer")) {
      benchmark_runner(std::chrono::seconds(vm["timer"].as<int>()));
      return EXIT_SUCCESS;
    } else if (vm.count("count")) {
      benchmark_runner(vm["count"].as<int>());
      return EXIT_SUCCESS;
    } else {
      std::cout << desc << "\n";
      return EXIT_FAILURE;
    }
  }
};