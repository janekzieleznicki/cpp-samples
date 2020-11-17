#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <unordered_map>

struct SubstractedGenerator {
  struct Helper {
    constexpr std::uint_fast64_t operator()(std::size_t index) {
      if (index % 2) {
        return index + 2;
      } else {
        return std::floor((index + 1.0f) / 2.0f) + 1;
      }
    }
  };
  constexpr auto operator()(std::size_t index) {
    std::uint_fast64_t res{1};
    for (auto i{0U}; i < index; ++i) {
      res += Helper{}(i);
    }
    return res;
  }
};
struct Cycler {
  int num{4};
  bool operator--() {
    if (--num < 0)
      num = 3;
    return num > 1;
  }
};

template <std::integral Num, typename Res = Num> struct PartitionCount {
  std::unordered_map<Num, Res> calculated{{0U, 1U}, {1U, 1U}};
  constexpr auto operator()(Num number) {
    if (calculated.contains(number))
      return calculated.at(number);
    struct Counter {
      PartitionCount &partitioner;
      Cycler cycler{};
      Res res{0U};
      void operator()(Num const &i) {
        res += --cycler ? partitioner(i) : -partitioner(i);
      }
    };
    auto SubstractFromNumber = [number](Num i) -> Num { return number - i; };
    auto IsPositive = [](Num i) -> bool { return i >= 0; };
    auto [_, count] = std::ranges::for_each(
        std::views::iota(0) | std::views::transform(SubstractedGenerator{}) |
            std::views::transform(SubstractFromNumber) |
            std::views::take_while(IsPositive),
        Counter{*this});

    calculated[number] = count.res;
    // std::cout<< "\t Cache[ "<< number <<" ]: "<<count.res<<std::endl;
    return count.res;
  }
};