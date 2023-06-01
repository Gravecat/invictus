// death.hpp -- Death comes for us all eventually.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_DEATH_HPP_
#define UI_DEATH_HPP_

namespace invictus
{

class Death
{
public:
    static void die();  // The player has just died.
    static void render_death_screen();  // Renders the game over screen.

private:
    static uint8_t skull_pattern[7];    // The skull symbol to render on the game-over screen.
};

}       // namespace invictus
#endif  // UI_DEATH_HPP_
