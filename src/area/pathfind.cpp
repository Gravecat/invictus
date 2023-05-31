// area/pathfind.cpp -- Fairly simple implementation of A* pathfinding, with Manhattan/Euclidean methods.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "area/area.hpp"
#include "area/pathfind.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/prefs.hpp"
#include "entity/entity.hpp"
#include "tune/pathfind.hpp"


namespace invictus
{

/******************************
 * ASTARTILE CLASS DEFINITION *
 ******************************/

// Constructor.
AStarTile::AStarTile(int start_x, int start_y, int end_x, int end_y, float tile_cost) : end_x_(end_x), end_y_(end_y), start_x_(start_x), start_y_(start_y),
    tile_cost_(tile_cost)
{
    if (core()->prefs()->pathfind_euclidean()) heuristic_ = std::sqrt(std::pow(std::abs(start_x_ - end_x_), 2) + std::pow(std::abs(start_y_ - end_y_), 2));
    else heuristic_ = std::abs(start_x_ - end_x_) + std::abs(start_y_ - end_y_);
}

// Sets the tile cost (g) for this tile.
void AStarTile::set_tile_cost(int cost) { tile_cost_ = cost; }

// Retrieves the start_x coord.
int AStarTile::start_x() const { return start_x_; }

// Retrieves the start_y coord.
int AStarTile::start_y() const { return start_y_; }

// Retrieves the tile cost of this tile (g).
float AStarTile::tile_cost() const { return tile_cost_; }

// Gets the total cost of this tile (g + h).
float AStarTile::total_cost() const { return tile_cost_ + heuristic_; }


/*****************************
 * PATHFIND CLASS DEFINITION *
 *****************************/

// Sets default values.
Pathfind::Pathfind(PathfindMode mode, int start_x, int start_y, int end_x, int end_y) : end_x_(end_x), end_y_(end_y), mode_(mode), start_x_(start_x),
    start_y_(start_y) { }

// A* pathfinding algorithm.
std::vector<std::pair<int, int>> Pathfind::pathfind()
{
    std::vector<std::shared_ptr<AStarTile>> checked_tiles;  // A list of tiles that have already been checked.
    std::vector<std::shared_ptr<AStarTile>> viable_tiles;   // The current tiles that are still viable options.
    auto area = core()->game()->area();
    auto guru = core()->guru();
    if (LOG_PATHFINDING) guru->log("Attempting to pathfind from " + std::to_string(start_x_) + "," + std::to_string(start_y_) + " to " +
        std::to_string(end_x_) + "," + std::to_string(end_y_));

    // Add the starting tile to the list.
    checked_tiles.push_back(std::make_shared<AStarTile>(start_x_, start_y_, end_x_, end_y_, 0));

    auto find_candidates = [this, &area, &checked_tiles, &guru, &viable_tiles](std::shared_ptr<AStarTile> current_tile = nullptr) -> int
    {
        int new_candidates = 0;
        float travel_cost = 0, current_cost = 0;
        int current_x = start_x_, current_y = start_y_;
        if (current_tile)
        {
            current_cost = current_tile->tile_cost();
            current_x = current_tile->start_x();
            current_y = current_tile->start_y();
        }

        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                if (x == 0 && y == 0) continue;
                travel_cost = ((x == 0 || y == 0) ? PATHFIND_TRAVEL_COST_STRAIGHT : PATHFIND_TRAVEL_COST_DIAGONAL);

                // Check if the tile we're investigating is already on the list.
                if (current_tile)
                {
                    bool valid = true;
                    int check_x = current_tile->start_x() + x, check_y = current_tile->start_y() + y;
                    for (auto tile : checked_tiles)
                    {
                        if (tile->start_x() == check_x && tile->start_y() == check_y)
                        {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid) continue;
                }

                // Add this tile to the potentials list.
                int coord_x = current_x + x, coord_y = current_y + y;
                auto possible_tile = std::make_shared<AStarTile>(coord_x, coord_y, end_x_, end_y_, current_cost + travel_cost);

                checked_tiles.push_back(possible_tile);
                const bool blocks_movement = area->tile(coord_x, coord_y)->tag(TileTag::BlocksMovement);
                bool add_to_viable = true;
                if (blocks_movement)
                {
                    possible_tile->set_tile_cost(BIG_NUMBER);
                    if (LOG_PATHFINDING) guru->log("Marking invalid tile: " + std::to_string(coord_x) + "," + std::to_string(coord_y));
                    continue;
                }
                for (auto entity : *area->entities())
                {
                    if (entity->blocks_tile(coord_x, coord_y))
                    {
                        if (mode_ == PathfindMode::PATHFIND_MOBILE && entity->type() == EntityType::PLAYER) continue;
                        if (mode_ == PathfindMode::PATHFIND_MOBILE && entity->type() == EntityType::MOBILE)
                        {
                            possible_tile->set_tile_cost(current_cost + travel_cost + PATHFIND_ALLY_BLOCKER_COST);
                            continue;
                        }
                        possible_tile->set_tile_cost(BIG_NUMBER);
                        add_to_viable = false;
                        break;
                    }
                }
                if (!add_to_viable) continue;
                if (LOG_PATHFINDING) guru->log("Marking potential tile: " + std::to_string(coord_x) + "," + std::to_string(coord_y));
                viable_tiles.push_back(possible_tile);
                new_candidates++;
            }
        }
        
        return new_candidates;
    };

    // Add the initial up/down/left/right positions to the lists.
    int new_candidates = find_candidates();
    if (LOG_PATHFINDING) guru->log(std::to_string(new_candidates) + " initial candidates.");

    // Loop until we run out of viable tiles to check.
    bool found_destination = false;
    int tries = 0;
    std::shared_ptr<AStarTile> current_tile = nullptr;
    while (viable_tiles.size() > 0 && tries++ < PATHFIND_MAX_TRIES)
    {
        // Attempt to find the best possible option.
        float lowest_score = BIG_NUMBER;
        for (auto tile : viable_tiles)
        {
            if (tile->start_x() == end_x_ && tile->start_y() == end_y_)
            {
                current_tile = tile;
                found_destination = true;
                break;
            }
            else if (tile->total_cost() < lowest_score)
            {
                current_tile = tile;
                lowest_score = tile->total_cost();
            }
        }
        if (found_destination || lowest_score == BIG_NUMBER) break;

        for (unsigned int i = 0; i < viable_tiles.size(); i++)
        {
            if (viable_tiles.at(i) == current_tile)
            {
                viable_tiles.erase(viable_tiles.begin() + i);
                break;
            }
        }
        
        if (LOG_PATHFINDING) guru->log(std::to_string(viable_tiles.size()) + " tiles remain. Checking " + std::to_string(current_tile->start_x())  + "," +
            std::to_string(current_tile->start_y()) + " (score: " + std::to_string(current_tile->total_cost()) + ")");
        new_candidates = find_candidates(current_tile);
        if (LOG_PATHFINDING) guru->log(std::to_string(new_candidates) + " new candidates.");
    }

    std::vector<std::pair<int, int>> path;
    if (tries >= PATHFIND_MAX_TRIES)
    {
        if (LOG_PATHFINDING) guru->log("Could not find destination, aborting after " + std::to_string(PATHFIND_MAX_TRIES) + "+ tries.");
        return path;
    }

    if (found_destination)
    {
        if (LOG_PATHFINDING) guru->log("Found destination! Total travel score: " + std::to_string(current_tile->total_cost()));
        int current_x = end_x_, current_y = end_y_;
        path.push_back(std::pair<int, int>(end_x_, end_y_));
        tries = 0;
        int best_score = BIG_NUMBER;
        while ((current_x != start_x_ || current_y != start_y_) && tries++ < PATHFIND_MAX_TRIES)
        {
            std::shared_ptr<AStarTile> best_option = nullptr;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    if (x == 0 && y == 0) continue;
                    int here_x = current_x + x, here_y = current_y + y;
                    for (auto tile : checked_tiles)
                    {
                        if (tile->start_x() != here_x || tile->start_y() != here_y) continue;
                        else if (tile->tile_cost() < best_score)
                        {
                            best_score = tile->tile_cost();
                            best_option = tile;
                        }
                        break;
                    }
                }
            }
            if (!best_option)
            {
                if (LOG_PATHFINDING) guru->log("Could not determine next tile in backwalk!");
                path.clear();
                return path;
            }
            current_x = best_option->start_x();
            current_y = best_option->start_y();
            if (current_x != start_x_ || current_y != start_y_) path.insert(path.begin(), std::pair<int, int>(current_x, current_y));
        }
        if (LOG_PATHFINDING)
        {
            if (tries >= 1000)
            {
                guru->log("Unable to complete path backwalk. Length so far: " + std::to_string(path.size()) + ". Tiles checked: " +
                    std::to_string(checked_tiles.size()) + ".");
                guru->log("Start: " + std::to_string(start_x_) + "," + std::to_string(start_y_) + ". End: " + std::to_string(end_x_) + "," +
                    std::to_string(end_y_));
            }
            else guru->log("Path walked backwards, total length: " + std::to_string(path.size()) + ". Tiles checked: " + std::to_string(checked_tiles.size()) +
                ".");
        }
        if (tries >= 1000)
        {
            path.clear();
            return path;
        }
    }
    else if (LOG_PATHFINDING) guru->log("Could not find destination. :(");
    return path;
}

}   // namespace invictus
