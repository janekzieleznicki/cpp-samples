/**
Copied from reddit:
https://www.reddit.com/r/dailyprogrammer/comments/cn6gz5/20190807_challenge_380_intermediate_smooshed/ewcp2as/?utm_source=reddit&utm_medium=web2x&context=3
*/
#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

static const std::map<std::string, std::string> morse_map = std::map<std::string, std::string>{
	std::make_pair<std::string, std::string>(".-", "a"),
	std::make_pair<std::string, std::string>("-...", "b"),
	std::make_pair<std::string, std::string>("-.-.", "c"),
	std::make_pair<std::string, std::string>("-..", "d"),
	std::make_pair<std::string, std::string>(".", "e"),
	std::make_pair<std::string, std::string>("..-.", "f"),
	std::make_pair<std::string, std::string>("--.", "g"),
	std::make_pair<std::string, std::string>("....", "h"),
	std::make_pair<std::string, std::string>("..", "i"),
	std::make_pair<std::string, std::string>(".---", "j"),
	std::make_pair<std::string, std::string>("-.-", "k"),
	std::make_pair<std::string, std::string>(".-..", "l"),
	std::make_pair<std::string, std::string>("--", "m"),
	std::make_pair<std::string, std::string>("-.", "n"),
	std::make_pair<std::string, std::string>("---", "o"),
	std::make_pair<std::string, std::string>(".--.", "p"),
	std::make_pair<std::string, std::string>("--.-", "q"),
	std::make_pair<std::string, std::string>(".-.", "r"),
	std::make_pair<std::string, std::string>("...", "s"),
	std::make_pair<std::string, std::string>("-", "t"),
	std::make_pair<std::string, std::string>("..-", "u"),
	std::make_pair<std::string, std::string>("...-", "v"),
	std::make_pair<std::string, std::string>(".--", "w"),
	std::make_pair<std::string, std::string>("-..-", "x"),
	std::make_pair<std::string, std::string>("-.--", "y"),
	std::make_pair<std::string, std::string>("--..", "z")
}; 

bool starts_with(std::string const& str, std::string const& match)
{
	return std::mismatch(match.cbegin(), match.cend(), str.cbegin()).first == match.cend();
}

void find_permutations(std::string&& morse, std::vector<std::string>&& matched_chars, std::vector<std::vector<std::string>>& sequences)
{
	// If all characters have been matched, add the sequence of matches to the list.
	if (matched_chars.size() >= 26)
	{
		sequences.push_back(matched_chars);
		return;
	}

	// For each morse letter... 
	std::for_each(morse_map.cbegin(), morse_map.cend(),
		[&morse, &matched_chars, &sequences](std::pair<std::string, std::string> const& morse_pair) mutable
	{
		// If the letter hasn't already been matched...
		if (std::find(matched_chars.cbegin(), matched_chars.cend(), morse_pair.second) != matched_chars.cend())
			return;

		// And if the input morse sequence begins with the letter...
		if (starts_with(morse, morse_pair.first))
		{
			// Add the letter to the list of matches...
			auto new_matches = matched_chars;
			new_matches.push_back(morse_pair.second);

			// And recursively try to match the start of the rest of the morse string. 
			find_permutations(morse.substr(morse_pair.first.size(), morse.size() - morse_pair.first.size()), std::move(new_matches), sequences);
		}
	});
}

int main()
{
	std::string test = ".--...-.-.-.....-.--........----.-.-..---.---.--.--.-.-....-..-...-.---..--.----..";
	std::vector<std::vector<std::string>> sequences;

	find_permutations(std::move(test), std::vector<std::string>(), sequences);

	std::for_each(sequences.cbegin(), sequences.cend(), 
		[](std::vector<std::string> const& sequence)
	{
		std::for_each(sequence.cbegin(), sequence.cend(),
			[](std::string const& letter)
		{
			std::cout << letter << " ";
		});
		std::cout << std::endl;
	});
}