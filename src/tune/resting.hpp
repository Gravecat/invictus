// tune/resting.hpp -- Preset values involving resting, and noises that are loud enough to wake the player.
// This value should always be within 0-100, the exception being the special value 255. When set to 255, a message is always shown regardless of whether
// the player is sleeping or not, but will never cause them to wake.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_RESTING_HPP_
#define TUNE_RESTING_HPP_

namespace invictus
{

// The % chance of waking up...
constexpr int   AWAKEN_CHANCE_DROP_ITEM =           20;     // ...due to the player dropping an item.
constexpr int   AWAKEN_CHANCE_MOB_CLOSE_DOOR =      40;     // ...when a mobile within line-of-sight closes a door.
constexpr int   AWAKEN_CHANCE_MOB_DEATH_FAR =       40;     // ...due to a mobile dying outside of line-of-sight.
constexpr int   AWAKEN_CHANCE_MOB_DEATH_NEAR =      80;     // ...due to a mobile dying within line-of-sight.
constexpr int   AWAKEN_CHANCE_MOB_DROP_ITEM =       2;      // ...when a mobile within line-of-sight drops an item.
constexpr int   AWAKEN_CHANCE_MOB_OPEN_DOOR =       30;     // ...when a mobile within line-of-sight opens a door.
constexpr int   AWAKEN_CHANCE_MOB_TAKE_ITEM =       1;      // ...when a mobile within line-of-sight picks up an item.
constexpr int   AWAKEN_CHANCE_MOBS_FIGHTING_NEAR =  80;     // ...from mobiles fighting each other nearby.

}       // namespace invictus
#endif  // TUNE_RESTING_HPP_
