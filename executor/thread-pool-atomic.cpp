#include "Common.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

template <size_t pool_size = 2>
struct ThreadPoolExecutor : BaseExecutor<ThreadPoolExecutor<pool_size>> {
private:
  std::atomic<CountingExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<CountingExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  // std::atomic<long long> counter{0};

  std::vector<std::thread> threads{pool_size};

public:
  static const size_t size = pool_size;
  ThreadPoolExecutor() {
    auto work = [this] {
      long long counter{0};
      while (continue_)
        try_execute(++counter);
    };
    for (auto &thr : threads) {
      thr = std::thread{work};
    }
  }
  ~ThreadPoolExecutor() {
    stop();
    for (auto &thr : threads)
      thr.join();
  }
  void stop() { continue_.store(false); }
  void try_execute(long long counter) {
    CountingExecutable *expected{nullptr}, *empty{nullptr};
    while (!maybe_executable.compare_exchange_weak(expected, empty))
      ;
    if (expected != nullptr)
      (*expected)(counter);
  }
  void emplace(CountingExecutable *new_work) {
    CountingExecutable *empty{nullptr};
    while (!maybe_executable.compare_exchange_weak(empty, new_work))
      empty = nullptr; // with complex tasks we should check if executor is not
                       // doing something important
  }
};

template <> struct BenchmarkRunner<ThreadPoolExecutor<>> {
  std::unique_ptr<BaseExecutor<ThreadPoolExecutor<>>> executor;
  std::mutex cout_mutex;
  int num_threads =
      std::thread::hardware_concurrency() - ThreadPoolExecutor<>::size;
  void operator()(int count) {
    std::cout << PrintableResult::column_names() << std::endl;
    {
      using namespace std::chrono_literals;
      auto work_per_thread = std::div(count, num_threads);
      auto rem = work_per_thread.rem;
      std::vector<std::thread> threads(num_threads);
      for (auto &th : threads)
        th = std::thread(CountBenchmark<ThreadPoolExecutor<>>{*executor,
                                                                cout_mutex,
                                                                std::cout},
                         work_per_thread.quot + ((--rem >= 0) ? 1 : 0));
      for (auto &th : threads)
        th.join();
    }
  }
  void operator()(std::chrono::seconds sec) {
    std::cout << PrintableResult::column_names() << std::endl;
    {
      using namespace std::chrono_literals;

      std::vector<std::thread> threads(num_threads);
      for (auto &th : threads)
        th = std::thread(
            DurationBenchmark<ThreadPoolExecutor<>, std::chrono::nanoseconds>{
                *executor, cout_mutex, std::cout, sec});
      for (auto &th : threads)
        th.join();
    }
  }
};

int main(int argc, char **argv) {
  CmdlineParser<ThreadPoolExecutor<>> clp;
  return clp(argc, argv);
}