// util/strx.hpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_STRX_HPP_
#define UTIL_STRX_HPP_

#include <cstdint>
#include <string>


namespace invictus
{

class StrX
{
public:
    static std::string  bool_to_str(bool b);    // Converts a bool to a string ("true" or "false").
                        // Find and replace one string with another.
    static bool         find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace);
    static std::string  itoh(uint32_t num, uint8_t min_len);    // Converts an integer into a hex string.
    static bool         str_to_bool(std::string str);           // Converts a string to a bool.
    static std::string  str_tolower(std::string str);           // Converts a string to lower-case.
    static std::string  str_toupper(std::string str);           // Converts a string to upper-case.
    static std::vector<std::string> string_explode(std::string str, const std::string &separator);  // String split/explode function.
};

}       // namespace invictus
#endif  // UTIL_STRX_HPP_
