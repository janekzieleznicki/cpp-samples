#include "Common.hpp"

#include <iostream>
#include <thread>

struct AtomicExecutor : BaseExecutor<AtomicExecutor> {
private:
  std::atomic<MyExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<MyExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  int counter{0};

public:
  void stop() { continue_.store(false); }
  void try_execute(int &counter) {
    MyExecutable *expected{nullptr}, *empty{nullptr};
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
  void emplace(MyExecutable *new_work) {
    MyExecutable *empty{nullptr};
    while (!maybe_executable.compare_exchange_weak(empty, new_work))
      empty = nullptr;
  }
};

int main() {
  std::unique_ptr<BaseExecutor<AtomicExecutor>> executor =
      std::make_unique<AtomicExecutor>();
  // AtomicExecutor executor;
  std::thread executor_thread{[&executor = *executor] { executor(); }};
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 8196; ++i) {
      MyExecutable executable{};
      auto future = executable.promise.get_future();
      executor->emplace(&executable);
      auto res = future.get();
      std::cout << PrintableResult{res, start} << std::endl;
    }
  }
  executor->stop();
  executor_thread.join();
}