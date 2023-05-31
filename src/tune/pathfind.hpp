// tune/pathfind.hpp -- Values which can be adjusted to change the behaviour of the pathfinding system. Tinker with caution!
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_PATHFIND_HPP_
#define TUNE_PATHFIND_HPP_

namespace invictus
{

constexpr bool  LOG_PATHFINDING =               false;  // Logs pathfinding attempts in the Guru log.
constexpr int   PATHFIND_ALLY_BLOCKER_COST =    10;     // The pathfind cost for an allied NPC blocking the route to the player.
                                                        // A higher number here will make NPCs less patient and more likely to find an alternate route.
constexpr int   PATHFIND_MAX_TRIES =            1000;   // The maximum amount of tries before giving up on a pathfind attempt.
constexpr float PATHFIND_TRAVEL_COST_DIAGONAL = 2.0f;   // The travel cost for diagonal movement.
constexpr float PATHFIND_TRAVEL_COST_STRAIGHT = 1.0f;   // The travel cost for horizontal or vertical movement.

}       // namespace invictus
#endif  // TUNE_PATHFIND_HPP_
