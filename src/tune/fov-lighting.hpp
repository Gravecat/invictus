// tune/fov-lighting.hpp -- Tune values for field-of-view, line-of-sight and static/dynamic lighting.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_FOV_LIGHTING_HPP_
#define TUNE_FOV_LIGHTING_HPP_

namespace invictus
{

static constexpr int    PLAYER_FOV_BONUS_HIGH =     10; // When using a high light source, the field of view is set to the light source's power, plus this.
static constexpr int    PLAYER_FOV_BONUS_LOW =      1;  // When using a low light source, the field of view is set to the light source's power, plus this.
static constexpr int    PLAYER_FOV_BONUS_MEDIUM =   5;  // When using a medium light source, the field of view is set to the light source's power, plus this.
static constexpr int    PLAYER_FOV_LIGHT_HIGH =     8;  // A light source counts as high (for PLAYER_FOV_BONUS_HIGH) when this level or above.    
static constexpr int    PLAYER_FOV_LIGHT_MEDIUM =   4;  // A light source counts as medium (for PLAYER_FOV_BONUS_MEDIUM) when this level or above;
                                                        // below, it'll count for PLAYER_FOV_BONUS_LOW.
static constexpr int    PLAYER_FOV_MINIMUM =        3;  // The field of view when the player has no light source.

}       // namespace invictus
#endif  // TUNE_FOV_LIGHTING_HPP_
