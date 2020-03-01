#include <chrono>
#include <future>
#include <ostream>
#include <string>

struct ExecutionResult {
  std::chrono::high_resolution_clock::time_point time_point;
  std::string data;
};

struct PrintableResult {
  ExecutionResult execution_result;
  std::chrono::high_resolution_clock::time_point start_time_point;

  friend std::ostream &operator<<(std::ostream &os,
                                  const PrintableResult &exec);
};
std::ostream &operator<<(std::ostream &os, const PrintableResult &exec) {
  os << '['
     << std::chrono::duration_cast<std::chrono::nanoseconds>(
            exec.execution_result.time_point - exec.start_time_point)
            .count()
     << " ns]: " << exec.execution_result.data;
  return os;
}
template <typename Result> struct Executable {
  std::promise<Result> promise;
  template <typename... Args> void operator()(Args &&... args) {
    promise.set_value(Result{std::forward<Args>(args)...});
  }
};
template <> struct Executable<ExecutionResult> {
  std::promise<ExecutionResult> promise;
  void operator()(int counter) {
    std::string output{"Iteration"};
    output.append(std::to_string(counter));
    promise.set_value(ExecutionResult{std::chrono::high_resolution_clock::now(),
                                      std::move(output)});
  }
};

using MyExecutable = Executable<ExecutionResult>;

template <typename DerivedExecutor> struct BaseExecutor {
  void operator()() { static_cast<DerivedExecutor &> (*this)(); }
  void emplace(MyExecutable *new_work) {
    static_cast<DerivedExecutor &>(*this).emplace(new_work);
  }
  void stop() { static_cast<DerivedExecutor &>(*this).stop(); }
  virtual ~BaseExecutor() = default;
};