// misc/timing.hpp -- All definitions for timing in the game (i.e. how long actions take to perform).
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef MISC_TIMING_HPP_
#define MISC_TIMING_HPP_

namespace invictus
{

class Timing
{
public:
    static constexpr float  TICK_SPEED =            0.1f;   // How often does the heartbeat tick? (Don't change this. I mean it.)
    static constexpr float  TIME_BASE_MOVEMENT =    1.0f;   // The basic movement speed for moving a single tile, unmodified by armour or other attributes.
                                                            // Probably best to not adjust this one, it might make things weird.
    static constexpr float  TIME_CLOSE_DOOR =       0.5f;   // The time it takes to close a door.
    static constexpr float  TIME_DO_NOTHING =       1.0f;   // The time spent when pressing the 'do nothing' key.
    static constexpr float  TIME_OPEN_DOOR =        0.5f;   // The time it takes to open a door.
};

}       // namespace invictus
#endif  // MISC_TIMING_HPP_
