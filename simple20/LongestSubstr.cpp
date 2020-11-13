#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>

using namespace std::literals;
struct Data {
  const std::string_view string;
  const std::uint32_t count;
};

Data data[]{
    {"abcabcbb"sv, 3},
    {"bbbbb"sv, 1},
    {"pwwkew"sv, 3},
    {""sv, 0},
};
std::size_t span_impl(std::string str) {
  std::uint_fast32_t ans{0};

  return 0;
}

std::size_t longest_nonzero(std::string str) {
  std::size_t max_len{0};
  for (auto iter = begin(str); iter != end(str); ++iter) {
    for (auto inner_iter = next(iter); inner_iter != end(str); ++inner_iter) {
      std::size_t len = std::distance(iter, inner_iter);
      if (max_len >= len)
        continue;
      std::vector<char> chars{iter, inner_iter}, uniques{};
      std::ranges::sort(chars);
      std::ranges::unique_copy(chars, std::back_inserter(uniques));
      if (len == uniques.size())
        max_len = len;
      std::cout << "\t Len:" << len << " Input: ";
      std::ranges::copy(iter, inner_iter,
                        std::ostream_iterator<char>(std::cout, " "));
      std::cout << " | Unique len: " << uniques.size() << " | ";
      std::ranges::copy(uniques, std::ostream_iterator<char>(std::cout, " "));
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
  return max_len;
}
int longest_substr(const std::string_view str) {
  std::vector<int> diff;
  diff.reserve(str.size());
  std::cout << "STR_: ";
  std::ranges::copy(str, std::ostream_iterator<char>(std::cout, " "));
  std::cout << std::endl;
  return longest_nonzero(std::string{str});
}
int main(int, char *[]) {
  for (auto const &[str, expected] : data) {
    auto val = longest_substr(str);
    std::cout << (val == expected ? "Passed " : "Failed ")
              << "Expected: " << expected << " Received: " << val << std::endl;
  }
}

