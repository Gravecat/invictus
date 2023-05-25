// entity/player.cpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use the same stats for combat etc.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "entity/player.hpp"


namespace invictus
{

// Constructor.
Player::Player() : Mobile()
{
    set_ascii('@');
    set_colour(Colour::WHITE_BOLD);
    set_light_power(4);
    set_name("player");
}

// Calculates the player's field-of-view radius.
uint16_t Player::fov_radius() const
{
    const int lp = light_power();
    if (lp >= FOV_LIGHT_HIGH) return lp + FOV_BONUS_HIGH;
    else if (lp >= FOV_LIGHT_MEDIUM) return lp + FOV_BONUS_MEDIUM;
    else if (!lp) return FOV_MINIMUM;
    else return std::max<int>(lp + FOV_BONUS_LOW, FOV_MINIMUM);
}

}   // namespace invictus
