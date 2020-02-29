#include <chrono>
#include <future>
#include <iostream>
#include <thread>

template <typename Result> struct Executable {
  std::promise<Result> promise;
  template <typename... Args> void operator()(Args &&... args) {
    promise.set_value(Result{std::forward<Args>(args)...});
  }
};
struct ExecutionResult {
  std::chrono::high_resolution_clock::time_point time_point;
  std::string data;
};

using MyExecutable = Executable<ExecutionResult>;
struct Executor {
private:
  std::atomic<MyExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<MyExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  std::mutex mutex;
public:
  void stop() { continue_.store(false); }
  void try_execute(int &counter) {
    
    MyExecutable *expected{nullptr};
    {
      std::lock_guard guard{mutex};
      expected = maybe_executable.load();
      maybe_executable.store(nullptr);
    }
    if (expected == nullptr);
      // std::this_thread::yield();
    else {
      std::string output{"Iteration"};
      output.append(std::to_string(++counter));
      (*expected)(std::chrono::high_resolution_clock::now(), output);
    }
  }
  void operator()() {
    int counter{0};
    while (continue_) {
      try_execute(counter);
    }
  }
  void emplace(MyExecutable *new_work) {
    std::lock_guard guard{mutex};
    maybe_executable.store(new_work);
  }
};

int main() {
  Executor executor;
  std::thread executor_thread{[&executor] { executor(); }};
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 8196; ++i) {
      MyExecutable executable{};
      auto future = executable.promise.get_future();
      executor.emplace(&executable);
      auto res = future.get();
      std::cout << '['
                << std::chrono::duration_cast<std::chrono::nanoseconds>(
                       res.time_point - start)
                       .count()
                << " ns]: " << res.data << std::endl;
    }
  }
  executor.stop();
  executor_thread.join();
}