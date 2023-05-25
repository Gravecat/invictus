// util/strx.cpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <vector>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Converts a bool to a string ("true" or "false").
std::string StrX::bool_to_str(bool b) { return (b ? "true" : "false"); }

// Find and replace one string with another.
bool StrX::find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace)
{
    std::string::size_type pos = 0;
    const std::string::size_type find_len = to_find.length(), replace_len = to_replace.length();
    if (find_len == 0) return false;
    bool found = false;
    while ((pos = input.find(to_find, pos)) != std::string::npos)
    {
        found = true;
        input.replace(pos, find_len, to_replace);
        pos += replace_len;
    }
    return found;
}

// Converts an integer into a hex string.
std::string StrX::itoh(uint32_t num, uint8_t min_len)
{
    std::stringstream ss;
    ss << std::hex << num;
    std::string hex = ss.str();
    while (min_len && hex.size() < min_len) hex = "0" + hex;
    return hex;
}

// Converts a string to a bool.
bool StrX::str_to_bool(std::string str)
{
    if (!str.size()) return false;
    switch (str[0])
    {
        case '0': case 'f': case 'F': case 'n': case 'N': case '-': return false;
        case '1': case 't': case 'T': case 'y': case 'Y': return true;
        default: core()->guru()->nonfatal("Invalid boolean string: " + str, Guru::GURU_ERROR); return false;
    }
}

// Converts a string to lower-case.
std::string StrX::str_tolower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// Converts a string to upper-case.
std::string StrX::str_toupper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// String split/explode function.
std::vector<std::string> StrX::string_explode(std::string str, const std::string &separator)
{
    std::vector<std::string> results;

    std::string::size_type pos = str.find(separator, 0);
    const size_t pit = separator.length();

    while(pos != std::string::npos)
    {
        if (pos == 0) results.push_back("");
        else results.push_back(str.substr(0, pos));
        str.erase(0, pos + pit);
        pos = str.find(separator, 0);
    }
    results.push_back(str);

    return results;
}

}   // namespace invictus
