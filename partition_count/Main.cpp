#include "sources/PartitionCount.hpp"

#include "absl/numeric/int128.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <ranges>

struct NamedTime {
  const std::string_view name;
  const std::chrono::microseconds time;
};
struct Timed {
  const std::string_view name;
  template <typename... Args>
  std::pair<auto, std::chrono::microseconds> operator()(Args &&...args) {

    auto start = std::chrono::high_resolution_clock::now();
    auto val = std::invoke(std::forward<Args>(args)...);
    return std::make_pair(
        val, std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::high_resolution_clock::now() - start));
  }
};

int main(int, char *[]) {
  using Partitioner = PartitionCount<std::int64_t, absl::uint128>;
  // Partitioner{}(66);
  Partitioner partitioner{};
  std::vector vec{66, 90, 666, 1458};
  for (int i : vec) {
    auto [val, time] = Timed{}(std::ref(partitioner),i);
    std::cout << "Partition [ " << i << " ] took " << time.count()
              << " us: " << val << std::endl;
  }
}
