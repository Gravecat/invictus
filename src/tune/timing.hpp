// tune/timing.hpp -- All definitions for timing in the game (i.e. how long actions take to perform).
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_TIMING_HPP_
#define TUNE_TIMING_HPP_

namespace invictus
{

constexpr float TICK_SPEED =                0.1f;   // How often does the heartbeat tick? (Don't change this. I mean it.)
constexpr float TIME_BASE_MOVEMENT =        1.0f;   // The basic movement speed for moving a single tile, unmodified by armour or other attributes.
                                                    // Probably best to not adjust this one, it might make things weird.
constexpr float TIME_CLOSE_DOOR =           0.5f;   // The time it takes to close a door.
constexpr float TIME_DO_NOTHING =           1.0f;   // The time spent when pressing the 'do nothing' key.
constexpr float TIME_DROP_ITEM =            0.5f;   // The time it takes to drop an item on the floor.
constexpr float TIME_EQUIP_ARMOUR_BODY =    5.0f;   // The time it takes to equip body armour.
constexpr float TIME_EQUIP_ARMOUR_FEET =    2.0f;   // The time it takes to equip boots.
constexpr float TIME_EQUIP_ARMOUR_HANDS =   2.0f;   // The time it takes to equip gloves.
constexpr float TIME_EQUIP_ARMOUR_HEAD =    1.0f;   // The time it takes to equip a helmet.
constexpr float TIME_EQUIP_SHIELD =         2.0f;   // The time it takes to equip a shield.
constexpr float TIME_EQUIP_WEAPON =         0.4f;   // The time it takes to equip a weapon.
constexpr float TIME_OPEN_DOOR =            0.5f;   // The time it takes to open a door.
constexpr float TIME_TAKE_ITEM =            1.0f;   // The time it takes to pick up an item from the ground.
constexpr float TIME_UNEQUIP_ARMOUR_BODY =  5.0f;   // The time it takes to unequip body armour.
constexpr float TIME_UNEQUIP_ARMOUR_FEET =  2.0f;   // The time it takes to unequip boots.
constexpr float TIME_UNEQUIP_ARMOUR_HANDS = 2.0f;   // The time it takes to unequip gloves.
constexpr float TIME_UNEQUIP_ARMOUR_HEAD =  1.0f;   // The time it takes to unequip a helmet.
constexpr float TIME_UNEQUIP_SHIELD =       1.0f;   // The time it takes to unequip a shield.
constexpr float TIME_UNEQUIP_WEAPON =       0.5f;   // The time it takes to unequip a weapon.

}       // namespace invictus
#endif  // TUNE_TIMING_HPP_
