#pragma once
#include <algorithm>
#include <random>
#include <vector>

class WorkSimulator {
private:
  std::random_device dev;
  std::minstd_rand0 engine{dev()};
public:
template<typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
  long long work(){
    std::uniform_int_distribution<Int> uniform_dist(std::numeric_limits<Int>::min(), std::numeric_limits<Int>::max());
    std::vector<Int> vec(1024 * 1024 * 8);
    for (auto &val : vec)
      val = uniform_dist(engine);
    return std::accumulate(vec.begin(), vec.end(), 0);
  }
};