#include "Common.hpp"

#include <iostream>
#include <thread>

struct MutexExecutor : BaseExecutor<MutexExecutor> {
private:
  std::atomic<MyExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<MyExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  std::mutex mutex;
  int counter{0};

public:
  void stop() { continue_.store(false); }
  void try_execute(int &counter) {
    MyExecutable *expected{nullptr};
    {
      std::lock_guard guard{mutex};
      expected = maybe_executable.load();
      maybe_executable.store(nullptr);
    }
    if (expected != nullptr)
      (*expected)(counter);
  }
  void operator()() {
    while (continue_) {
      try_execute(++counter);
    }
  }
  void emplace(MyExecutable *new_work) {
    std::lock_guard guard{mutex};
    maybe_executable.store(new_work);
  }
};

int main() {

  std::unique_ptr<BaseExecutor<MutexExecutor>> executor =
      std::make_unique<MutexExecutor>();
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