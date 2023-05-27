// dev/acs-display.hpp -- Accessible by launching the game with the `-acs-display` parameter.
// Displays the ACS glyphs, for determining what terminals can support which glyphs.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef DEV_ACS_DISPLAY_HPP_
#define DEV_ACS_DISPLAY_HPP_

namespace invictus
{

class DevACSDisplay
{
public:
    static void display_test(); // Displays the ACS glyphs.
};

}       // namespace invictus
#endif  // DEV_ACS_DISPLAY_HPP_
