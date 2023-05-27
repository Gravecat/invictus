// tune/timing.hpp -- All definitions for timing in the game (i.e. how long actions take to perform).
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_TIMING_HPP_
#define TUNE_TIMING_HPP_

namespace invictus
{

static constexpr float  TICK_SPEED =            0.1f;   // How often does the heartbeat tick? (Don't change this. I mean it.)
static constexpr float  TIME_BASE_MOVEMENT =    1.0f;   // The basic movement speed for moving a single tile, unmodified by armour or other attributes.
                                                        // Probably best to not adjust this one, it might make things weird.
static constexpr float  TIME_CLOSE_DOOR =       0.5f;   // The time it takes to close a door.
static constexpr float  TIME_DO_NOTHING =       1.0f;   // The time spent when pressing the 'do nothing' key.
static constexpr float  TIME_DROP_ITEM =        0.5f;   // The time it takes to drop an item on the floor.
static constexpr float  TIME_OPEN_DOOR =        0.5f;   // The time it takes to open a door.
static constexpr float  TIME_TAKE_ITEM =        1.0f;   // The time it takes to pick up an item from the ground.

}       // namespace invictus
#endif  // TUNE_TIMING_HPP_
