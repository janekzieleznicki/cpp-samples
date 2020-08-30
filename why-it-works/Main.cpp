#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

class Convoluteted {
  using Tricky = std::map<std::string, std::pair<long, unsigned long long>>;
  Tricky tricky;

public:
  Convoluteted()
      : tricky{
            {"data1", {100, 100 * 100}},
            {"data2", {200, 200 * 200}},
            {"data3", {300, 300 * 300}},
        } {}
  void print_sorted(std::ostream &os) {
    std::vector<std::pair<std::string, std::pair<long, unsigned long long>>>
        vec{std::make_move_iterator(std::begin(tricky)),
            std::make_move_iterator(std::end(tricky))};
    std::sort(std::begin(vec), std::end(vec),
              [](auto const &left, auto const &right) {
                return left.second.second < right.second.second;
              });
    std::for_each(std::begin(vec), std::end(vec), [&os](auto const &data) {
      os << data.first << ":\t{ " << data.second.first << '\t'
         << data.second.second << " }\n";
    });
  }
};

int main() {
  Convoluteted convoluted;
  convoluted.print_sorted(std::cout);
  convoluted.print_sorted(std::cout);
}