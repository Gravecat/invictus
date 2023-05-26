// tune/area-generation.hpp -- Hard-coded values which are used during area/dungeon generation, and can be tweaked to adjust the outcome.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_AREA_GENERATION_HPP_
#define TUNE_AREA_GENERATION_HPP_

namespace invictus
{

static constexpr bool   AREA_GEN_DEBUG_MESSAGES = false;    // Enable debug dungeon generation messages in the Guru log.
static constexpr int    DUNGEON_MAX_WALKABLE =          50; // The maximum % of the map that needs to be walkable (i.e. floor) to be considered viable.
                                                            // Too much of this can be bad, as it leads to cluttered maps.
static constexpr int    DUNGEON_MIN_WALKABLE =          20; // The minimum % of the map that needs to be walkable (i.e. floor) to be considered viable.
                                                            // Too little of this can be bad, as it leads to very small maps.
static constexpr int    DUNGEON_ROOM_CORNER_SMOOTHING = 5;  // The lower this number, the more rooms will have smooth edges rather than being square.
                                                            // Set to 1 for full smoothing, 0 for none.
static constexpr int    DUNGEON_ROOM_GEN_RETRIES =      10; // How many new rooms are generated after a room was unable to be inserted in the map?
                                                            // The higher this number, the better quality maps will be generated, but it'll be much slower.
static constexpr int    DUNGEON_ROOM_SIZE_MAX =         12; // The largest room allowed by the map generator.
static constexpr int    DUNGEON_ROOM_SIZE_MIN =         6;  // The smallest room allowed by the map generator.
static constexpr int    DUNGEON_ROOM_FLOOR_TILES_MIN =  10; // Generated rooms must have at least this many floor tiles.
static constexpr int    TOMB_WALL_TOMB_CHANCE =         5;  // 1 in X chance of a viable wall location being dug out into an alcove for a burial tomb.

}       // namespace invictus
#endif  // TUNE_AREA_GENERATION_HPP_
