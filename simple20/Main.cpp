#include <coroutine>
#include <iostream>

template <typename Type> struct generator {
  struct promise_type;
  using handle = std::coroutine_handle<promise_type>;
  struct promise_type {
    Type current_value;
    auto get_return_object() { return generator{handle::from_promise(*this)}; }
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() { return std::suspend_always{}; }
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
    auto yield_value(Type value) {
      current_value = value;
      return std::suspend_always{};
    }
  };
  bool move_next() { return coro ? (coro.resume(), !coro.done()) : false; }
  Type& current_value() { return coro.promise().current_value; }
  generator(generator const &) = delete;
  generator(generator &&rhs) : coro(rhs.coro) { rhs.coro = nullptr; }
  ~generator() {
    if (coro)
      coro.destroy();
  }

private:
  generator(handle h) : coro(h) {}
  handle coro;
};

generator<std::string> f() {
  co_yield "one-value";
  co_yield "two-value";
}

int main() {
  auto g = f();
  while (g.move_next()) {
    std::cout << g.current_value() << std::endl;
    std::cout << g.current_value() << std::endl;
  }
}