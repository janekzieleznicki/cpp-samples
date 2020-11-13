#include <algorithm>
#include <iostream>
#include <string_view>

using std::literals::operator""sv;
struct Test {
  const std::string_view left;
  const std::string_view right;
  const bool expected;
};

Test test[]{
    {"nicole"sv, "icolen"sv, true},
    {"nicole"sv, "lenico"sv, true},
    {"nicole"sv, "coneli"sv, false},
    {"aabaaaaabaab"sv, "aabaabaabaaa"sv, true},
    {"abc"sv, "cba"sv, false},
    {"xxyyy"sv, "xxxyy"sv, false},
    {"xyxxz"sv, "xxyxz"sv, false},
    {"x"sv, "x"sv, true},
    {"x"sv, "xx"sv, false},
    {"x"sv, ""sv, false},
    {""sv, ""sv, true},
};

bool is_neklace(const std::string_view lhs, const std::string_view rhs) {
  if (lhs.empty() && rhs.empty())
    return true;
  std::string left{lhs};

  for (int i = 0; i < lhs.size(); ++i) {
    std::rotate(begin(left), std::next(begin(left)), end(left));
    if (std::string_view{left} == rhs)
      return true;
  }
  return false;
}

int main(int, char *[]) {
  for (const auto &[left, right, expected] : test) {
    std::cout << "L: " << left << " R: " << right << "\t"
              << (is_neklace(left, right) == expected ? "Passed" : "Failed")
              << std::endl;
  }

  return 0;
}
