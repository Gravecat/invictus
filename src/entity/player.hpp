// entity/player.hpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use the same stats for combat etc.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_PLAYER_HPP_
#define ENTITY_PLAYER_HPP_

#include "entity/mobile.hpp"


namespace invictus
{

class Player : public Mobile
{
public:
                Player();   // Constructor.
    uint16_t    fov_radius() const; // Calculates the player's field-of-view radius.
    EntityType  type() const { return EntityType::PLAYER; } // Self-identifier function.

private:
    static constexpr int    FOV_BONUS_HIGH =    10; // When using a high light source, the field of view is set to the light source's power, plus this value.
    static constexpr int    FOV_BONUS_LOW =     1;  // When using a low light source, the field of view is set to the light source's power, plus this value.
    static constexpr int    FOV_BONUS_MEDIUM =  5;  // When using a medium light source, the field of view is set to the light source's power, plus this value.
    static constexpr int    FOV_LIGHT_HIGH =    8;  // A light source counts as high (for FOV_BONUS_HIGH) when this level or above.
    static constexpr int    FOV_LIGHT_MEDIUM =  4;  // A light source counts as medium (for FOV_BONUS_MEDIUM) when this level or above;
                                                    // below, it'll count for FOV_BONUS_LOW.
    static constexpr int    FOV_MINIMUM =       3;  // The field of view when the player has no light source.
};

}       // namespace invictus
#endif  // ENTITY_PLAYER_HPP_
