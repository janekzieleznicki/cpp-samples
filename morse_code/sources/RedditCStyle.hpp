/**
Copied from reddit:
https://www.reddit.com/r/dailyprogrammer/comments/cn6gz5/20190807_challenge_380_intermediate_smooshed/ew9hx3u/?utm_source=reddit&utm_medium=web2x&context=3


*/

#pragma once 

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <tuple>

constexpr std::array<char, 26> alphabet
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

constexpr std::array<const char*, 26> alpha_encodings
{ ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--.." };

/*******************************************************************************
 * I store the morse alphabet in a tree structure on the stack using an array as
 * a pool to allocate nodes from. Each node stores, optionally, a character;
 * and 8-bit indices of the children (in the array that holds the tree). There
 * will only ever be 27 nodes so the 8 bit indices are fine. -1 acts as a
 * sentinel value to indicate that there is no child branch.
 *
 * This tree can additionally be constructed at compile time, so we can have the
 * Morse alphabet encoded into our binary.
 ******************************************************************************/

constexpr int8_t NO_CHILDREN = -1;

// The nodes should each fit in 4 bytes when padded; then I expect the whole
// tree to take 27*4 = 108 bytes.
struct MorseNode
{
    char c;
    int8_t dot;
    int8_t dash;

    constexpr MorseNode() : c('\0'), dot(NO_CHILDREN), dash(NO_CHILDREN) {}
};

struct MorseTree
{
    std::array<MorseNode, 27> buf;
    int8_t used;

    constexpr MorseTree() : buf{ MorseNode() }, used(1) {}
};

constexpr void add_encoding(MorseTree& tree, int8_t root, char a, const char* str)
{
    if (str[0] == '\0')
    {
        if (tree.buf[root].c != '\0')
            throw std::logic_error("Multiple encodings");
        tree.buf[root].c = a;
    }
    else
    {
        auto next_index = str[0] == '.' ? tree.buf[root].dot : tree.buf[root].dash;
        if (next_index == NO_CHILDREN)
        {
            tree.buf[tree.used] = MorseNode();
            next_index = tree.used;
            str[0] == '.' ? (tree.buf[root].dot = next_index) : (tree.buf[root].dash = next_index);
            tree.used += 1;
        }
        add_encoding(tree, next_index, a, str+1);
    }
}

constexpr void add_encoding(MorseTree& tree, char a, const char* str)
{
    add_encoding(tree, 0, a, str);
}

constexpr MorseTree build_tree(std::array<char, 26> as, std::array<const char*, 26> es)
{
    MorseTree tree;
    for (int i = 0; i < 26; ++i)
    {
        add_encoding(tree, as[i], es[i]);
    }
    return tree;
}

// This is the whole morse alphabet encoded in a tree, available at compile
// time.
constexpr MorseTree morse_tree = build_tree(alphabet, alpha_encodings);

/******************************************************************************/

auto all_encodings(const char* str)
{
    int8_t node = 0;
    std::array<std::tuple<char, int8_t>, 4> found;
    int8_t nfound = 0;
    int8_t len = 0;

    while (str[len] != '\0')
    {
        node = str[len] == '.' ? morse_tree.buf[node].dot : morse_tree.buf[node].dash;

        if (node == NO_CHILDREN) { break; }
        len += 1;
        
        if (morse_tree.buf[node].c != '\0')
        {
            assert(nfound < 4);
            found[nfound++] = std::tuple(morse_tree.buf[node].c, len);
        }
    }

    return std::tuple(found, nfound);
}

/*******************************************************************************
 * Using backtracking find a permutation of the alphabet that matches the given
 * string.
 ******************************************************************************/

bool is_used(uint32_t used, char c)
{
    return (used & (0x00000001 << (c - 'a'))) != 0;
}

uint32_t mark_used(uint32_t used, char c)
{
    return used | (0x00000001 << (c - 'a'));
}

bool find_permutation(char* dst, const char* str, uint32_t used = 0)
{
    if (str[0] == '\0') { return true; }

    auto [all, nenc] = all_encodings(str);

    for (auto i = nenc-1; i >= 0; --i)
    {
        auto [c, len] = all[i];
        if (is_used(used, c)) { continue; }

        if (find_permutation(dst+1, str+len, mark_used(used, c)))
        {
            dst[0] = c;
            return true;
        }
    }
    return false;
}

