#pragma once
#include <string_view>

using std::string_view_literals::operator""sv;

struct Test {
  std::string_view str;
  std::string_view morse;
};

constexpr Test decode_test[]{
    {"oqmgztyknxcdbjruiwalsfhvep"sv,
     "-----.-----.--..--.---.--.-..--.-.-..-....---.-...-...--.-.-.......-........-..--."sv},
    {"wirnbfzehatqlojpgcvusyxkmd"sv,
     ".--...-.-.-.....-.--........----.-.-..---.---.--.--.-.-....-..-...-.---..--.----.."sv},
    {"wzjlepdsvothqfxkbgrmyicuna"sv,
     ".----...---.-....--.-........-----....--.-..-.-..--.--...--..-.---.--..-.-...--..-"sv},
    {"uvfsqmjazxthbidyrkcwegponl"sv,
     "..-...-..-....--.---.---.---..-..--....-.....-..-.--.-.-.--.-..--.--..--.----..-.."sv},
};
constexpr Test test[]{
    {"e"sv, "."},
    {"sos"sv, "...---..."},
    {"daily"sv, "-...-...-..-.--"},
    {"programmer"sv, ".--..-.-----..-..-----..-."},
    {"bits"sv, "-.....-..."},
    {"three"sv, "-.....-..."},
};