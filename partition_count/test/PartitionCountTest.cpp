#include <cstdint>
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "PartitionCount.hpp"

#include "catch2/catch.hpp"

#include <vector>

unsigned int Factorial(unsigned int number) {
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Third generator", "[generators]") {
  std::vector<std::uint_fast32_t> numbers{
      1, 3, 2, 5, 3, 7, 4, 9, 5, 11, 6, 13, 7,
  };
  REQUIRE(SubstractedGenerator::Helper{}(0) == numbers[0]);
  REQUIRE(SubstractedGenerator::Helper{}(1) == numbers[1]);
  REQUIRE(SubstractedGenerator::Helper{}(2) == numbers[2]);
  REQUIRE(SubstractedGenerator::Helper{}(3) == numbers[3]);
  for (auto i{0u}; i < numbers.size(); ++i) {
    REQUIRE(SubstractedGenerator::Helper{}(i) == numbers[i]);
  }
}
TEST_CASE("Substracted generator", "[generators]") {
  std::vector<std::uint_fast32_t> numbers{1,  2,  5,  7,  12, 15, 22,
                                          26, 35, 40, 51, 57, 70};
  REQUIRE(SubstractedGenerator{}(0) == numbers[0]);
  REQUIRE(SubstractedGenerator{}(1) == numbers[1]);
  REQUIRE(SubstractedGenerator{}(2) == numbers[2]);
  REQUIRE(SubstractedGenerator{}(3) == numbers[3]);
  for (auto i{0u}; i < numbers.size(); ++i) {
    REQUIRE(SubstractedGenerator{}(i) == numbers[i]);
  }
}
TEST_CASE("CyclerTest", "[partitions]") {
  Cycler cycler;
  for (int i = 0; i < 100; ++i) {
    REQUIRE(--cycler == true);
    REQUIRE(--cycler == true);
    REQUIRE(--cycler == false);
    REQUIRE(--cycler == false);
  }
}
TEST_CASE("Partitioner", "[partitions]") {
  using Partitioner = PartitionCount<std::int_fast64_t>;
  REQUIRE(Partitioner{}(0) == 1U);
  REQUIRE(Partitioner{}(1) == 1U);
  REQUIRE(Partitioner{}(2) == 2U);
  REQUIRE(Partitioner{}(3) == 3U);
  REQUIRE(Partitioner{}(4) == 5U);
  REQUIRE(Partitioner{}(5) == 7U);
  REQUIRE(Partitioner{}(6) == 11U);
}
TEST_CASE("Partitioner2", "[partitions]") {
  using Partitioner = PartitionCount<std::int_fast64_t>;
  REQUIRE(Partitioner{}(66) == 2323520);
}

