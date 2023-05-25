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
    void    load();     // Loads user prefs from a file, if it exists.
    void    save();     // Saves user prefs to a file.
    bool    use_colour() const; // Check if using colour is allowed.

private:
    std::string filename_;      // The filename of the user prefs file.
    bool        use_colour_;    // Is colour enabled?
};

}       // namespace invictus
#endif  // CORE_PREFS_HPP_
