// util/strx.hpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_STRX_HPP_
#define UTIL_STRX_HPP_

#include <cstdint>
#include <string>


class StrX
{
public:
                        // Find and replace one string with another.
    static bool         find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace);
    static std::string  itoh(uint32_t num, uint8_t min_len);    // Converts an integer into a hex string.
    static std::string  str_toupper(std::string str);           // Converts a string to upper-case.
};

#endif  // UTIL_STRX_HPP_
