#include <chrono>
#include <future>
#include <ostream>
#include <sstream>
#include <string>
static const int NUM_WORKS = 2048;
struct ExecutionResult {
  std::chrono::high_resolution_clock::time_point time_point;
  std::string data;
};

struct PrintableResult {
    static std::string column_names(){
        std::stringstream ss;
        ss << "Time[ns]\tIteration\tIndex";
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
    int index;
  std::promise<ExecutionResult> promise;
  void operator()(int counter) {
    std::string output{"\t"};
    output.append(std::to_string(counter));
    output.append("\t");
    output.append(std::to_string(index));
    promise.set_value(ExecutionResult{std::chrono::high_resolution_clock::now(),
                                      std::move(output)});
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