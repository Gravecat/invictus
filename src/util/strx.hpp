// util/strx.hpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_STRX_HPP_
#define UTIL_STRX_HPP_

#include <cstdint>
#include <string>
#include <vector>


namespace invictus
{

// Flags used in StrX::comma_list().
constexpr int   CL_FLAG_USE_AND =   1;
constexpr int   CL_FLAG_SQL_MODE =  2;


class StrX
{
public:
    static std::string  bool_to_str(bool b);    // Converts a bool to a string ("true" or "false").
                        // Find and replace one string with another.
    static std::string  capitalize_first_letter(std::string str);   // Capitalizes the first letter of a string.
    static uint32_t     center_strvec(std::vector<std::string> &vec);   // Centers all the strings in a vector.
    static std::string  comma_list(std::vector<std::string> vec, unsigned int flags);   // Converts a vector to a comma-separated list.
    static bool         find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace);
    static std::string  ftos(double num);           // Converts a float or double to a string.
    static std::string  intostr_pretty(int num);    // Returns a 'pretty' version of a number in string format, such as "12,345".
    static bool         is_vowel(char ch);          // Checks if a character is a vowel.
    static std::string  itoh(uint32_t num, uint8_t min_len);    // Converts an integer into a hex string.
    static std::string  number_to_word(unsigned long long number);  // Converts small numbers into words.
    static std::string  pad_string(const std::string &str, unsigned int min_len, bool ansi = false);    // Pads a string to a given length.
    static std::string  pad_string_center(const std::string &str, unsigned int min_len, bool ansi = false); // As above, but centers the string.
    static std::string  possessive_string(const std::string &str);  // Makes a string into a possessive noun (e.g. orc = orc's, platypus = platypus')
    static std::string  rainbow_text(const std::string &str, const std::string &colours);   // Makes pretty rainbow text!
    static bool         str_to_bool(std::string str);   // Converts a string to a bool.
    static std::string  str_tolower(std::string str);   // Converts a string to lower-case.
    static std::string  str_toupper(std::string str);   // Converts a string to upper-case.
    static std::vector<std::string> string_explode(std::string str, const std::string &separator);  // String split/explode function.
    // Similar to string_explode(), but takes colour tags into account, and wraps to a given line length.
    static std::vector<std::string> string_explode_colour(const std::string &str, unsigned int line_len);
    // Returns a count of the amount of times a string is found in a parent string.
    static unsigned int strlen_colour(const std::string &str);  // Returns the length of a string, taking colour tags into account.
    static unsigned int word_count(const std::string &str, const std::string &word);
};

}       // namespace invictus
#endif  // UTIL_STRX_HPP_
