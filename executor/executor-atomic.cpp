#include "Common.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

struct AtomicExecutor : BaseExecutor<AtomicExecutor> {
private:
  std::atomic<CountingExecutable *> maybe_executable{nullptr};
  static_assert(std::atomic<CountingExecutable *>::is_always_lock_free);
  std::atomic<bool> continue_{true};
  static_assert(std::atomic<bool>::is_always_lock_free);
  long long counter{0};

public:
  void stop() { continue_.store(false); }
  void try_execute(long long &counter) {
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
    static std::mutex cerr_protector;
    while (!maybe_executable.compare_exchange_weak(empty, new_work)) {
      if (empty != nullptr) {
        std::lock_guard cerr_guard{cerr_protector};
        std::cerr << "[ " << std::hex << std::this_thread::get_id()
                  << " ]\tSomeone already placed work:\t" << std::hex << empty
                  << std::endl;
      }
      std::this_thread::yield();
      empty = nullptr; // with complex tasks we should check if executor is not
                       // doing something important
    }
  }
};

int main(int argc, char **argv) {
  CmdlineParser<AtomicExecutor> clp;
  return clp(argc, argv);
}