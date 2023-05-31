// area/pathfind.hpp -- Fairly simple implementation of A* pathfinding, with Manhattan/Euclidean methods.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef AREA_PATHFIND_HPP_
#define AREA_PATHFIND_HPP_

#include <cstdint>
#include <utility>
#include <vector>


namespace invictus
{

enum class PathfindMode : uint8_t { PATHFIND_PLAYER, PATHFIND_MONSTER };


// Data struct used with A* pathfinding calculations.
class AStarTile
{
public:
            AStarTile(int start_x, int start_y, int end_x, int end_y, float tile_cost);// Constructor.
    void    set_tile_cost(int cost);    // Sets the tile cost (g) for this tile.
    int     start_x() const;    // Retrieves the start_x coord.
    int     start_y() const;    // Retrieves the start_y coord.
    float   tile_cost() const;  // Retrieves the tile cost of this tile (g).
    float   total_cost() const; // Gets the total cost of this tile (g + h).

private:
    int     end_x_, end_y_;     // Coordinates of the destination tile.
    float   heuristic_;         // The estimated (heuristic) cost of this tile (h).
    int     start_x_, start_y_; // Coordinates of the starting tile.
    float   tile_cost_;         // The cost so far to travel through this tile (g).
};


// The main pathfinding routine.
class Pathfind
{
public:
                                        Pathfind(PathfindMode mode, int start_x, int start_y, int end_x, int end_y);    // Sets default values.
    std::vector<std::pair<int, int>>    pathfind(); // A* pathfinding algorithm.

private:
    static constexpr int    BIG_NUMBER = 999999999; // A really big number.

    int             end_x_, end_y_;     // The ending X,Y coordinates.
    PathfindMode    mode_;              // The pathfinding mode in use.
    int             start_x_, start_y_; // The starting X,Y coordinates.
};

}       // namespace invictus
#endif  // AREA_PATHFIND_HPP_
