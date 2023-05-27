// dev/keycode-check.hpp -- Accessible by launching the game with the `-keycode-check` parameter.
// Debug/testing code to check user inputs from Curses, and report unknown keycodes or escape sequences.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef DEV_KEYCODE_CHECK_HPP_
#define DEV_KEYCODE_CHECK_HPP_

namespace invictus
{

class DevKeycodeCheck
{
public:
    static void begin();    // Begins the keycode checking loop.
};

}       // namespace invictus
#endif  // DEV_KEYCODE_CHECK_HPP_
