// core/prefs.hpp -- User-defined preferences, which can be set in userdata/prefs.txt
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_PREFS_HPP_
#define CORE_PREFS_HPP_

#include <string>


namespace invictus
{

class Prefs
{
public:
            Prefs(std::string filename);    // Constructor, sets default values.
    uint8_t acs_flags() const;          // Retrieves the ACS glyph usage flags.
    void    load();                     // Loads user prefs from a file, if it exists.
    bool    pathfind_euclidean() const; // Is the pathfinding code using the Euclidean method (true) or the Manhattan method (false)?
    void    save();                     // Saves user prefs to a file.
    bool    use_colour() const;         // Check if using colour is allowed.

private:
    uint8_t     acs_flags_;             // The ACS glyph usage flags.
    std::string filename_;              // The filename of the user prefs file.
    bool        pathfind_euclidean_;    // Does the pathfinding code use the Euclidean method (as opposed to the Manhattan method)?
    bool        use_colour_;            // Is colour enabled?
};

}       // namespace invictus
#endif  // CORE_PREFS_HPP_
