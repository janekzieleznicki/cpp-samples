#include "sources/Data.hpp"
#include "sources/MorseCode.hpp"
#include "sources/RedditC.hpp"
#include "sources/RedditCStyle.hpp"
#include <bits/c++config.h>
#include <chrono>
#include <future>
void separate_output() {
  std::ranges::copy(std::string{"\n=======================\n"},
                    std::ostream_iterator<char>(std::cout, ""));
}
void reddit_func(const std::string_view str) {
  char src[128];
  char dst[128] = {0};
  std::ranges::fill(src, '\0');
  std::ranges::copy(std::string{str}, std::begin(src));
  auto linelen = std::max(std::size_t{0L}, str.length());

  if (linelen == 0) {
    return;
  }

  std::cout << src << '\t';
  find_permutation(dst, src);
  std::cout << dst << '\n';
}

int reddit() {
  int i;
  for (i = 0; i < EXPECTED_IN_LEN; i++) {
    best[i] = morse_symbols[SYMBOLS_N - 1];
  }
  best[EXPECTED_IN_LEN] = '\0';
  input[EXPECTED_IN_LEN] = '\0';
  generate_input(0);
  return EXIT_SUCCESS;
}

void morser_output() {
  for (const auto &[str, morse] :
       test | std::views::reverse | std::views::take(1)) {
    auto smorsed = Smorse{}.encode(str);
    std::cout << "L: " << str << " res: " << smorsed;
    std::cout << "\t" << (smorsed == morse ? "Passed" : "Failed") << std::endl;
  }

  separate_output();
  std::cout << "\nDecode:\n\n";
  separate_output();

  for (const auto &[str, morse] : decode_test | std::views::reverse) {
    auto start = std::chrono::high_resolution_clock::now();
    auto decoded = Smorse{}.decode(morse);
    std::cout << "Found possibles: " << decoded.size() << std::endl;
    if (std::ranges::find(decoded, str) == end(decoded)) {
      std::cerr << "Expected not found in results!" << std::endl;
    } else {
      std::cout << "Congratulations, successful decode" << std::endl;
    }
    std::cout << "It took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::high_resolution_clock::now() - start)
                     .count()
              << "milliseconds" << std::endl;
    separate_output();
  }
  separate_output();
  std::cout
      << Smorse{}
             .decode(
                 "......-..--...---.-....---...--....--.-..---.....---.-.---..---.-....--.-.---.-.--"sv)
             .size()
      << " <= Should be 41" << std::endl;
  separate_output();
}
struct NamedTime {
  const std::string_view name;
  const std::chrono::milliseconds time;
};
struct Timed {
  const std::string_view name;
  template <typename... Args> NamedTime operator()(Args &&...args) {

    auto start = std::chrono::high_resolution_clock::now();
    std::invoke(std::forward<Args>(args)...);
    return {name, std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now() - start)};
  }
};

void print(std::string_view who, std::chrono::milliseconds time) {
  std::cout << "It took " << who << " " << time.count() << " ms" << std::endl;
}

int main(int, char *[]) {
  std::future<NamedTime> futures[]{
      std::async(std::launch::async, Timed{"Reddit"sv},
                 [] {
                   for (const auto &[str, morse] :
                        decode_test | std::views::reverse)
                     reddit_func(morse);
                 }),
      std::async(std::launch::async, Timed{"Smorse"sv},
                 [] {
                   for (const auto &[str, morse] :
                        decode_test | std::views::reverse)
                     auto decoded = Smorse{}.decode(morse);
                 }),
  };
  std::ranges::for_each(futures, [](auto const &future) { future.wait(); });
  std::ranges::for_each(futures, [](auto &future) {
    auto res = future.get();
    print(res.name, res.time);
  });
}
