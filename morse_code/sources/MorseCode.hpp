#pragma once
#include <algorithm>
#include <bits/c++config.h>
#include <chrono>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

using std::string_view_literals::operator""sv;
struct Codes {
  struct Code {
    const std::string_view morse;
    const char letter;
  };

  static constexpr Code codes[]{
      {".-"sv, 'a'},   {"-..."sv, 'b'}, {"-.-."sv, 'c'}, {"-.."sv, 'd'},
      {"."sv, 'e'},    {"..-."sv, 'f'}, {"--."sv, 'g'},  {"...."sv, 'h'},
      {".."sv, 'i'},   {".---"sv, 'j'}, {"-.-"sv, 'k'},  {".-.."sv, 'l'},
      {"--"sv, 'm'},   {"-."sv, 'n'},   {"---"sv, 'o'},  {".--."sv, 'p'},
      {"--.-"sv, 'q'}, {".-."sv, 'r'},  {"..."sv, 's'},  {"-"sv, 't'},
      {"..-"sv, 'u'},  {"...-"sv, 'v'}, {".--"sv, 'w'},  {"-..-"sv, 'x'},
      {"-.--"sv, 'y'}, {"--.."sv, 'z'},
  };
  constexpr std::string_view operator[](char c) {
    auto code = std::ranges::find(codes, c, &Code::letter);
    return code->morse;
  }
  constexpr char operator[](const std::string_view morsed) {
    auto code = std::ranges::find(codes, morsed, &Code::morse);
    return code->letter;
  }
};

class Smorse {
  struct PossibleChars {
    const std::string_view encoded;
    std::vector<char> chars{};
    void operator()(Codes::Code const &code) {
      if (std::ranges::equal(encoded | std::views::take(code.morse.size()),
                             code.morse))
        chars.emplace_back(code.letter);
    }
  };
  struct IsAlphabetPermutation {
    static constexpr auto alphabet = "wirnbfzehatqlojpgcvusyxkmd"sv;
    bool operator()(const std::string_view str) {
      return std::is_permutation(begin(alphabet), end(alphabet), begin(str),
                                 end(str));
    }
  };
  std::vector<std::string> decode_impl(const std::string_view encoded,
                                       const std::string matched) {
    auto [_, candidates] =
        std::ranges::for_each(Codes::codes, PossibleChars{encoded});

    std::vector<std::string> result;
    for (char c : candidates.chars) {
      if (std::ranges::find(matched, c) != end(matched)) {
        continue;
      }
      auto new_matched = matched + c;
      auto left_to_process = encoded.substr(Codes{}[c].size());
      if (left_to_process.empty()) {
        result.emplace_back(std::string{c});
      } else {
        auto nexts = decode_impl(left_to_process, new_matched);
        std::ranges::for_each(
            nexts | std::views::all,
            [before = std::string{c}, &result](auto const &after) {
              result.emplace_back(before + after);
            });
      }
    }
    return result;
  }

public:
  std::string encode(std::string_view string) {
    std::stringstream ss;
    std::ranges::transform(string,
                           std::ostream_iterator<std::string_view>(ss, ""),
                           [](char c) { return Codes{}[c]; });
    return ss.str();
  }
  template<bool should_print=false>
  std::vector<std::string> decode(const std::string_view encoded) {
    std::cout << "Decoding : " << encoded << std::endl;
    auto res = decode_impl(encoded, "");
    if constexpr(should_print){
      if (res.empty())
        std::cout << "No match found";
      else
        std::ranges::copy(
            res, std::ostream_iterator<std::string_view>(std::cout, " | "));
      std::cout << std::endl;
    }
    auto filtered_res = res | std::views::filter(IsAlphabetPermutation{});
    return {begin(filtered_res), end(filtered_res)};
  }
};