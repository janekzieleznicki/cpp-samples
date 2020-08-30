#include <coroutine>
#include <iostream>
#include <string>
#include <vector>

struct Printer {
  std::string data;
  template<typename Stringable>
  Printer(Stringable&& str) : data{std::forward<Stringable>(str)} {
    std::cout << "Printer(string)\t"<<data<<"\n";
  }
  Printer(Printer &&right) : data{std::move(right.data)} {
    std::cout << "Printer::moved\t"<<data<<"\n";
  }
  Printer(Printer const &right) : data{right.data} {
    std::cout << "Printer::copied\t"<<data<<"\n";
  }
  operator std::string const&() const {
    std::cout << "Printer::conversion to string\t"<<data<<"\n";
    return data;
  }
};

int main() {
  std::vector<Printer> printers; printers.reserve(100);
  std::cout<<"Push from initializer list\n";
  printers.push_back({"one"});
  std::cout << "--------------------------------\n";
  std::cout<<"push from temporary\n";
  printers.push_back(Printer{"two"});
  std::cout << "--------------------------------\n";
  std::cout<<"emplace from const char*\n";
  printers.emplace_back("three");
  std::cout << "--------------------------------\n";
  std::cout<<"emplace from string\n";
  printers.emplace_back(std::string{"four"});
  std::cout << "--------------------------------\n";
  std::cout<<"emplace from temporary\n";
  printers.emplace_back(Printer{"five"});
}