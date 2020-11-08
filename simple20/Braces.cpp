#include <iostream>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <string>
#include <string_view>
#include <numeric>
#include <concepts>

using namespace std::literals;
struct TestParam{
	const std::string_view string;
	const bool expected;
};
TestParam strings[]{
	{"((()))"sv,true},
		{"()()()"sv, true},
		{"())("sv,false},
		{")(()"sv, false},
		{")))((("sv, false},
		{"())(())()"sv, false}
};
struct ToNumeric{
	int operator()(char c){
		switch (c){
			case '(':return 1;
	case ')':return -1;
		}
	}
};
template<std::integral T>
struct NonNegative{
	bool operator()(T rhs){
		return std::greater_equal<T>{}(rhs,0);
	}
};
bool is_valid(const std::string_view string){
	std::ranges::copy(string, std::ostream_iterator<char>(std::cout));
	std::cout<<std::endl;
	std::vector<int> ints{};
	ints.reserve(string.size());
	std::transform_inclusive_scan(begin(string),end(string),std::back_inserter(ints),std::plus<int>{},ToNumeric{});
	std::ranges::copy(ints, std::ostream_iterator<int>(std::cout,"|"));
	auto check = [](int val){return val >= 0;};
	if(std::ranges::find_if_not(ints, NonNegative<int>{})==end(ints)){
		return (ints.back()==0);
	}
	return false;

}

int main(){

for (auto const& [s, expected]: strings){
	std::cout << "String: ";
	std::cout << (is_valid(s) == expected ? "Passed" : "Failed");
	std::cout << "\n";
}
	return 0;
}
