// core/strx.hpp -- Various utility functions that deal with string manipulation/conversion.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_STRX_HPP_
#define CORE_STRX_HPP_
#include "core/invictus.hpp"


class StrX
{
public:
    static bool find_and_replace(std::string &input, const std::string &to_find, const std::string &to_replace);    // Find and replace one string with another.
};

#endif  // CORE_STRX_HPP_
