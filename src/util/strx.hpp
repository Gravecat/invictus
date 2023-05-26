// util/strx.hpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_STRX_HPP_
#define UTIL_STRX_HPP_

#include <cstdint>
#include <string>
#include <vector>


namespace invictus
{

class StrX
{
public:
    static constexpr int    CL_FLAG_USE_AND =   1;
    static constexpr int    CL_FLAG_SQL_MODE =  2;

    static std::string  bool_to_str(bool b);    // Converts a bool to a string ("true" or "false").
                        // Find and replace one string with another.
    static std::string  comma_list(std::vector<std::string> vec, unsigned int flags);   // Converts a vector to a comma-separated list.
    static bool         find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace);
    static std::string  ftos(double num);                       // Converts a float or double to a string.
    static std::string  itoh(uint32_t num, uint8_t min_len);    // Converts an integer into a hex string.
    static bool         str_to_bool(std::string str);           // Converts a string to a bool.
    static std::string  str_tolower(std::string str);           // Converts a string to lower-case.
    static std::string  str_toupper(std::string str);           // Converts a string to upper-case.
    static std::vector<std::string> string_explode(std::string str, const std::string &separator);  // String split/explode function.
    // Similar to string_explode(), but takes colour and high/low-ASCII tags into account, and wraps to a given line length.
    static std::vector<std::string> string_explode_colour(const std::string &str, unsigned int line_len);
    // Returns a count of the amount of times a string is found in a parent string.
    static unsigned int strlen_colour(const std::string &str);  // Returns the length of a string, taking colour and high/low-ASCII tags into account.
    static unsigned int word_count(const std::string &str, const std::string &word);
};

}       // namespace invictus
#endif  // UTIL_STRX_HPP_
