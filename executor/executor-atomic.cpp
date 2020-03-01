#include "Common.hpp"

#include <iostream>
#include <thread>
#include <vector>

struct AtomicExecutor : BaseExecutor<AtomicExecutor> {
private:
  std::atomic<CountingExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<CountingExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  int counter{0};

public:
  void stop() { continue_.store(false); }
  void try_execute(int &counter) {
    CountingExecutable *expected{nullptr}, *empty{nullptr};
    while (!maybe_executable.compare_exchange_weak(expected, empty))
      ;
    if (expected != nullptr)
      (*expected)(counter);
  }
  void operator()() {
    while (continue_) {
      try_execute(++counter);
    }
  }
  void emplace(CountingExecutable *new_work) {
    CountingExecutable *empty{nullptr};
    while (!maybe_executable.compare_exchange_weak(empty, new_work))
      empty = nullptr; // with complex tasks we should check if executor is not
                       // doing something important
  }
};

int main() {
  std::unique_ptr<BaseExecutor<AtomicExecutor>> executor =
      std::make_unique<AtomicExecutor>();
  
  std::mutex cout_mutex;
  std::thread executor_thread{[&executor = *executor] { executor(); }};
  auto work = [&executor, &cout_mutex] {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_WORKS; ++i) {
      CountingExecutable executable{i};
      auto future = executable.promise.get_future();
      executor->emplace(&executable);
      auto res = future.get();
      std::lock_guard guard{cout_mutex};
      std::cout << PrintableResult{res, start} << std::endl;
    }
  };
  std::cout << PrintableResult::column_names() << std::endl;
  {
    std::vector<std::thread> threads(std::thread::hardware_concurrency() - 1);
    for (auto &th : threads)
      th = std::thread(work);
    for (auto &th : threads)
      th.join();
  }
  executor->stop();
  executor_thread.join();
}