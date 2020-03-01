#include "Common.hpp"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct MutexExecutor : BaseExecutor<MutexExecutor> {
private:
  //==============================================================================
  /*Work*/
  CountingExecutable *maybe_executable{nullptr};
  static_assert(std::atomic<CountingExecutable *>::is_always_lock_free);
  //==============================================================================
  /*Synchronization*/
  std::condition_variable cv;
  std::mutex mutex;
  //==============================================================================
  /*Internals*/
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);

  long long counter{0};

public:
  void stop() { continue_.store(false); }
  void try_execute(long long &counter) {
    CountingExecutable *expected{nullptr};
    {
      std::lock_guard guard{mutex};
      expected = maybe_executable;
      maybe_executable = nullptr;
    }
    if (expected != nullptr)
      (*expected)(counter);
  }
  void operator()() {
    while (continue_) {
      try_execute(++counter);
    }
  }
  void emplace(CountingExecutable *new_work) {
    while (true) {
      std::lock_guard guard{mutex};
      if (maybe_executable == nullptr) {
        maybe_executable = new_work;
        break;
      }
    }
  }
};

int main(int argc, char **argv) {
  CmdlineParser<MutexExecutor> clp;
  return clp(argc,argv);
}