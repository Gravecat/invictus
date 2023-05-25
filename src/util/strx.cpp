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

// Converts a float or double to a string.
std::string StrX::ftos(double num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
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

// Similar to string_explode(), but takes colour and high/low-ASCII tags into account, and wraps to a given line length.
std::vector<std::string> StrX::string_explode_colour(const std::string &str, unsigned int line_len)
{
    std::vector<std::string> output;

    // Check to see if the line of text has the no-split tag (ASCII character 0) at the start.
    if (str.size() >= 5)
    {
        if (!str.substr(0, 5).compare("^000^"))
        {
            output.push_back(str);
            return output;
        }
    }

    // Check to see if the line is too short to be worth splitting.
    if (strlen_colour(str) <= line_len)
    {
        output.push_back(str);
        return output;
    }

    // Split the string into individual words.
    std::vector<std::string> words = string_explode(str, " ");

    // Keep track of the current line and our position on it.
    unsigned int current_line = 0, line_pos = 0;
    std::string last_colour = "{w}";    // The last colour tag we encountered; white by default.

    // Start with an empty string.
    output.push_back("");

    for (auto word : words)
    {
        unsigned int length = word.length();    // Find the length of the word.

        // If the word includes high/low-ASCII tags, adjust the length.
        size_t htag_pos = word.find("^");
        bool high_ascii = false;
        if (htag_pos != std::string::npos)
        {
            if (word.size() > htag_pos + 4)
            {
                if (word.at(htag_pos + 4) == '^')
                {
                    length -= word_count(word, "^") * 2;
                    high_ascii = true;
                }
            }
        }

        const int colour_count = word_count(word, "{"); // Count the colour tags.
        if (colour_count) length -= (colour_count * 3); // Reduce the length if one or more colour tags are found.
        if (length + line_pos >= line_len)  // Is the word too long for the current line?
        {
            line_pos = 0; current_line++;   // CR;LF
            output.push_back(last_colour);  // Start the line with the last colour tag we saw.
        }
        if (colour_count)
        {
            // Duplicate the last-used colour tag.
            const std::string::size_type flo = word.find_last_of("{");
            if (flo != std::string::npos && word.size() >= flo + 3) last_colour = word.substr(flo, 3);
        }
        if (line_pos != 0)  // NOT the start of a new line?
        {
            length++;
            output.at(current_line) += " ";
        }

        // Is the word STILL too long to fit over a single line?
        // Don't attempt this on high/low-ASCII 'words'.
        while (length > line_len && !high_ascii)
        {
            const std::string trunc = word.substr(0, line_len);
            word = word.substr(line_len);
            output.at(current_line) += trunc;
            line_pos = 0;
            current_line++;
            output.push_back(last_colour);  // Start the line with the last colour tag we saw.
            length = word.size();   // Adjusts the length for what we have left over.
        }
        output.at(current_line) += word;
        line_pos += length;
    }

    return output;
}

// Returns the length of a string, taking colour and high/low-ASCII tags into account.
unsigned int StrX::strlen_colour(const std::string &str)
{
    unsigned int len = str.size();
    if (!len) return 0;

    // Count any colour tags.
    const int openers = std::count(str.begin(), str.end(), '{');
    const int symbols = std::count(str.begin(), str.end(), '^');
    if (openers) len -= openers * 3;
    if (symbols) len -= (symbols / 2) * 4;

    return len;
}

// Returns a count of the amount of times a string is found in a parent string.
unsigned int StrX::word_count(const std::string &str, const std::string &word)
{
    unsigned int count = 0;
    std::string::size_type word_pos = 0;
    while(word_pos != std::string::npos)
    {
        word_pos = str.find(word, word_pos);
        if (word_pos != std::string::npos)
        {
            count++;
            word_pos += word.length();
        }
    }
    return count;
}

}   // namespace invictus
