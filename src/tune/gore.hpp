// tune/gore.hpp -- These values affect the spread of blood and gore during combat.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_GORE_HPP_
#define TUNE_GORE_HPP_

namespace invictus
{

constexpr int   GORE_ALT_CHAR_CHANCE =      4;      // 1 in X chance that the highest level of gore will use the alternate character.
constexpr int   GORE_BLOODY_FEET_MAX =      3;      // The higher this number, the more gore will be treaded around when a mobile or the player walks through gore.
constexpr int   GORE_ON_MOBILE_DEATH =      10;     // The gore level generated when a mobile dies.
constexpr int   GORE_ON_PLAYER_DEATH =      100;    // The gore level generated when the player dies.
constexpr int   GORE_SPREAD_CHANCE_LOW =    10;     // The higher this value, the greater chance of a low gore splash spreading to nearby tiles.
constexpr int   GORE_SPREAD_CHANCE_MULTI =  20;     // The higher this value, the greater chance of a high gore splash spreading to nearby tiles.

}       // namespace invictus
#endif  // TUNE_GORE_HPP_
