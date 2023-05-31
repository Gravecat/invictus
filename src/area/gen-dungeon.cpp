// area/gen-dungeon.cpp -- The cool procedural dungeon area generator.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>
#include <functional>
#include <string>

#include "area/area.hpp"
#include "area/gen-dungeon.hpp"
#include "area/tile.hpp"
#include "codex/codex-monster.hpp"
#include "codex/codex-tile.hpp"
#include "core/core.hpp"
#include "core/guru.hpp"
#include "entity/monster.hpp"
#include "tune/area-generation.hpp"
#include "util/random.hpp"


namespace invictus
{

// Prepares a Map for procedural generation.
DungeonGenerator::DungeonGenerator(std::shared_ptr<Area> area_to_gen) : active_room_(-1), area_(area_to_gen), first_room_(true), stairs_up_room_(-1) { }

// Decorates a specified room.
void DungeonGenerator::decorate_room(unsigned int room_id)
{
    active_room_ = room_id;
    if (AREA_GEN_DEBUG_MESSAGES) core()->guru()->log("Decorating room " + std::to_string(room_id + 1) + " of " + std::to_string(rooms_.size()));

    decorate_room_druj_tombs(room_id);
}

// Room decoration: druj burial tombs.
bool DungeonGenerator::decorate_room_druj_tombs(unsigned int room_id)
{
    if (static_cast<int>(room_id) == stairs_up_room_) return false;
    bool success = false;
    int x, y, w, h;
    get_internal_room_size(room_id, &x, &y, &w, &h);

    auto apply_tomb = [this](int x, int y, int direction_x, int direction_y, int tomb_offset_x, int tomb_offset_y, int end_x, int end_y) -> bool {
        int search_x = x, search_y = y; // The current search coordinates.
        bool success = false;
        while (search_x != end_x || search_y != end_y)
        {
            int check_x = search_x + tomb_offset_x, check_y = search_y + tomb_offset_y;
            auto check_tile = area_->tile(check_x, check_y);

            // We want a tile that is a wall, with three walls, one floor as neighbour tiles.
            int diag_check_x = 0, diag_check_y = 0;
            if (std::abs(tomb_offset_x)) diag_check_y = 1; else diag_check_x = 1;
            if (check_tile->id() == TileID::LG_WALL && area_->tile(check_x - tomb_offset_x, check_y - tomb_offset_y)->id() == TileID::LG_FLOOR &&
                    neighbours(check_x, check_y, TileID::LG_WALL, false) >= 3 && neighbours(check_x, check_y, TileID::LG_FLOOR, false) == 1 &&
                    area_->tile(check_x + tomb_offset_x, check_y + tomb_offset_y)->id() == TileID::LG_WALL && area_->tile(check_x + tomb_offset_x +
                    diag_check_x, check_y + tomb_offset_y + diag_check_y)->id() == TileID::LG_WALL && area_->tile(check_x + tomb_offset_x - diag_check_x,
                    check_y + tomb_offset_y - diag_check_y)->id() == TileID::LG_WALL && Random::rng(TOMB_WALL_TOMB_CHANCE) == 1)
            {
                area_->set_tile(check_x, check_y, TileID::DRUJ_TOMB);
                success = true;
            }
            search_x += direction_x;
            search_y += direction_y;
        }
        return success;
    };

    // I am so fucking sorry.
    if (apply_tomb(x,         y,         1, 0,  0, -1, x + w,     y        )) success = true;
    if (apply_tomb(x,         y + h - 1, 1, 0,  0,  1, x + w,     y + h - 1)) success = true;
    if (apply_tomb(x,         y,         0, 1, -1,  0, x,         y + h    )) success = true;
    if (apply_tomb(x + w - 1, y,         0, 1,  1,  0, x + w - 1, y + h    )) success = true;

    return success;
}

// Gets the internal coordinates of a room, not counting doors.
void DungeonGenerator::get_internal_room_size(unsigned int room_id, int *x, int *y, int *w, int *h)
{
    *x = std::get<0>(rooms_.at(room_id));
    *y = std::get<1>(rooms_.at(room_id));
    *w = std::get<2>(rooms_.at(room_id));
    *h = std::get<3>(rooms_.at(room_id));

    bool top_has_doors = false, bottom_has_doors = false, left_has_doors = false, right_has_doors = false;
    int floor_count_top = 0, floor_count_bottom = 0;
    for (int cx = *x; cx < *x + *w; cx++)
    {
        auto top_tile = area_->tile(cx, *y), bottom_tile = area_->tile(cx, *y + *h - 1);
        if (top_tile->id() == TileID::LG_DOOR_CANDIDATE) top_has_doors = true;
        else if (top_tile->id() == TileID::LG_FLOOR) floor_count_top++;
        if (bottom_tile->id() == TileID::LG_DOOR_CANDIDATE) bottom_has_doors = true;
        else if (bottom_tile->id() == TileID::LG_FLOOR) floor_count_bottom++;
    }
    if (floor_count_top < 3) top_has_doors = true;
    if (floor_count_bottom < 3) bottom_has_doors = true;
    int floor_count_left = 0, floor_count_right = 0;
    for (int cy = *y; cy < *y + *h; cy++)
    {
        auto left_tile = area_->tile(*x, cy), right_tile = area_->tile(*x + *w - 1, cy);
        if (left_tile->id() == TileID::LG_DOOR_CANDIDATE) left_has_doors = true;
        else if (left_tile->id() == TileID::LG_FLOOR) floor_count_left++;
        if (right_tile->id() == TileID::LG_DOOR_CANDIDATE) right_has_doors = true;
        else if (right_tile->id() == TileID::LG_FLOOR) floor_count_right++;
    }
    if (floor_count_left < 3) left_has_doors = true;
    if (floor_count_right < 3) right_has_doors = true;

    if (bottom_has_doors) *h = *h - 1;
    if (right_has_doors) *w = *w - 1;
    if (top_has_doors) { *h = *h - 1; *y = *y + 1; }
    if (left_has_doors) { *w = *w - 1; *x = *x + 1; }
}

// Generates the new map!
void DungeonGenerator::generate()
{
    auto guru = core()->guru();
    int failed_rooms = 0;
    if (AREA_GEN_DEBUG_MESSAGES) guru->log("Beginning dungeon generation.");
    while (failed_rooms < DUNGEON_ROOM_GEN_RETRIES)
    {
        auto new_room = std::make_shared<Room>(this, Random::rng(DUNGEON_ROOM_SIZE_MIN, DUNGEON_ROOM_SIZE_MAX),
            Random::rng(DUNGEON_ROOM_SIZE_MIN, DUNGEON_ROOM_SIZE_MAX));
        new_room->generate(first_room_);

        if (first_room_)
        {
            int new_x = (area_->width() / 2) - (new_room->actual_width() / 2);
            int new_y = (area_->height() / 2) - (new_room->actual_height() / 2);
            if (!paste_room(new_room, new_x, new_y))
            {
                // We couldn't even paste the first room in? This should be impossible, but just in case, just quietly abort to avoid an infinite loop.
                void_map();
                generate();
                return;
            }
            rooms_.push_back(std::tuple<int, int, int, int>(new_x, new_y, new_room->actual_width(), new_room->actual_height()));
            active_room_ = 0;
            first_room_ = false;
            continue;
        }

        std::vector<std::pair<int, int>> possible_locations, map_link_points, room_link_points;

        // Look for all the available link points on the map right now.
        for (int x = 0; x < area_->width(); x++)
            for (int y = 0; y < area_->height(); y++)
                if (area_->tile(x, y)->id() == TileID::LG_DOOR_CANDIDATE && neighbours(x, y, TileID::VOID_TILE, false) >= 3)
                    map_link_points.push_back(std::pair<int, int>(x, y));
        if (!map_link_points.size()) break;

        // Look for all the available link points on the room.
        for (int x = 0; x < new_room->actual_width(); x++)
            for (int y = 0; y < new_room->actual_height(); y++)
                if (new_room->tile(x, y)->id() == TileID::LG_DOOR_CANDIDATE) room_link_points.push_back(std::pair<int, int>(x, y));
        if (!room_link_points.size())
        {
            // Again, this should actually be impossible, but just in case something weird happens, we want to avoid infinite loops.
            failed_rooms++;
            continue;
        }

        // Now we check each map link point against each room link point. This'll end up with a lot of potential link points, but it's still an order of
        // magnitude less than guessing random positions.
        for (auto mp : map_link_points)
            for (auto lp : room_link_points)
                possible_locations.push_back(std::pair<int, int>(mp.first - lp.first, mp.second - lp.second));

        // Now let's keep trying possible locations until one sticks, or until we run out.
        int found_x = -1, found_y = -1;
        while (possible_locations.size())
        {
            int choice = Random::rng(possible_locations.size()) - 1;
            auto loc = possible_locations.at(choice);
            if (paste_room(new_room, loc.first, loc.second))
            {
                found_x = loc.first;
                found_y = loc.second;
                failed_rooms = 0;   // Reset the failed room counter when a room successfully adds on.
                break;
            }
            possible_locations.erase(possible_locations.begin() + choice);
        }
        if (found_x < 0) failed_rooms++;
        else
        {
            rooms_.push_back(std::tuple<int, int, int, int>(found_x, found_y, new_room->actual_width(), new_room->actual_height()));
            active_room_ = rooms_.size() - 1;
        }
    }

    // Place the border walls, fill in the void space.
    int total_tiles = area_->width() * area_->height();
    int floor_tiles = total_tiles;
    for (int x = 0; x < area_->width(); x++)
    {
        for (int y = 0; y < area_->height(); y++)
        {
            if (x == 0 || x == area_->width() - 1 || y == 0 || y == area_->height() - 1)
            {
                area_->set_tile(x, y, TileID::LG_WALL);
                floor_tiles--;
            }
            else if (area_->tile(x, y)->id() == TileID::VOID_TILE)
            {
                area_->set_tile(x, y, TileID::LG_WALL);
                floor_tiles--;
            }
        }
    }
    int percentage_floor = std::round((static_cast<float>(floor_tiles) / static_cast<float>(total_tiles)) * 100.0f);

    active_room_ = 0;
    if (AREA_GEN_DEBUG_MESSAGES) guru->log("Basic map layout generated (" + std::to_string(percentage_floor) + "% walkable).");
    if (percentage_floor < DUNGEON_MIN_WALKABLE || percentage_floor > DUNGEON_MAX_WALKABLE)
    {
        if (AREA_GEN_DEBUG_MESSAGES) guru->log("Aborting.");
        void_map();
        generate();
        return;
    }

    // Remove any unlinked doors.
    int doors_changed = 0, total_doors_changed = 0, passes = 0;;
    do
    {
        passes++;
        doors_changed = 0;
        for (int x = 0; x < area_->width(); x++)
        {
            for (int y = 0; y < area_->height(); y++)
            {
                if (area_->tile(x, y)->id() == TileID::LG_DOOR_CANDIDATE)
                {
                    int n = neighbours(x, y, TileID::LG_FLOOR, false);
                    if (n < 2)  // Remove any doors that lead nowhere.
                    {
                        area_->set_tile(x, y, TileID::LG_WALL);
                        doors_changed++;
                    }
                    else if (n > 2) // Remove doors that are adjacent to other doors.
                    {
                        area_->set_tile(x, y, TileID::LG_FLOOR);
                        doors_changed++;
                    }
                    n = neighbours(x, y, TileID::LG_DOOR_CANDIDATE, true, 2);
                    if (n)  // Remove any doors that are too close to another door.
                    {
                        area_->set_tile(x, y, TileID::LG_FLOOR);
                        doors_changed++;
                    }
                }

                // Remove any 'nubs' left over from this cleanup.
                if (area_->tile(x, y)->id() == TileID::LG_FLOOR && neighbours(x, y, TileID::LG_WALL, false) == 3) area_->set_tile(x, y, TileID::LG_WALL);
            }
        }
        total_doors_changed += doors_changed;
    } while (doors_changed);
    if (total_doors_changed && AREA_GEN_DEBUG_MESSAGES) guru->log("Removed unlinked and duplicate door candidates (" + std::to_string(passes) + " passes, " +
        std::to_string(total_doors_changed) + " removed)");

    // Remove doors that are at an L-shape between two floor tiles.
    int l_doors_removed = 0;
    for (int x = 0; x < area_->width(); x++)
    {
        for (int y = 0; y < area_->height(); y++)
        {
            if (area_->tile(x, y)->id() != TileID::LG_DOOR_CANDIDATE) continue;
            if (area_->tile(x - 1, y)->id() == TileID::LG_FLOOR && area_->tile(x + 1, y)->id() == TileID::LG_FLOOR) continue;
            if (area_->tile(x, y - 1)->id() == TileID::LG_FLOOR && area_->tile(x, y + 1)->id() == TileID::LG_FLOOR) continue;
            area_->set_tile(x, y, TileID::LG_FLOOR);
            l_doors_removed++;
        }
    }
    if (l_doors_removed && AREA_GEN_DEBUG_MESSAGES) guru->log("Removed " + std::to_string(l_doors_removed) + " L-doors.");

    // Smooth the corners of the rooms a little.
    int corners_smoothed = 0;
    if (DUNGEON_ROOM_CORNER_SMOOTHING)
    {
        for (int x = 0; x < area_->width(); x++)
        {
            for (int y = 0; y < area_->height(); y++)
            {
                if (area_->tile(x, y)->id() == TileID::LG_FLOOR && neighbours(x, y, TileID::LG_WALL, true) >= 5 &&
                    Random::rng(DUNGEON_ROOM_CORNER_SMOOTHING) == 1)
                {
                    area_->set_tile(x, y, TileID::LG_WALL);
                    corners_smoothed++;
                }
            }
        }
        if (corners_smoothed && AREA_GEN_DEBUG_MESSAGES) guru->log("Smoothed " + std::to_string(corners_smoothed) + " wall corners.");
    }

    // Cutting out diagonals.
    int diagonals_removed = 0;
    bool diagonal_removed = false;
    do
    {
        diagonal_removed = false;
        for (int x = 0; x < area_->width() - 1; x++)
        {
            for (int y = 0; y < area_->height() - 1; y++)
            {
                if ((area_->tile(x, y)->id() == TileID::LG_WALL && area_->tile(x + 1, y)->id() == TileID::LG_FLOOR && area_->tile(x, y + 1)->id() ==
                    TileID::LG_FLOOR && area_->tile(x + 1, y + 1)->id() == TileID::LG_WALL) || (area_->tile(x, y)->id() == TileID::LG_FLOOR &&
                    area_->tile(x + 1, y)->id() == TileID::LG_WALL && area_->tile(x, y + 1)->id() == TileID::LG_WALL && area_->tile(x + 1, y + 1)->id() ==
                    TileID::LG_FLOOR))
                {
                    if (area_->tile(x, y)->id() == TileID::LG_FLOOR) { area_->set_tile(x, y, TileID::LG_WALL); diagonals_removed++; }
                    if (area_->tile(x + 1, y)->id() == TileID::LG_FLOOR) { area_->set_tile(x + 1, y, TileID::LG_WALL); diagonals_removed++; }
                    if (area_->tile(x, y + 1)->id() == TileID::LG_FLOOR) { area_->set_tile(x, y + 1, TileID::LG_WALL); diagonals_removed++; }
                    if (area_->tile(x + 1, y + 1)->id() == TileID::LG_FLOOR) { area_->set_tile(x + 1, y + 1, TileID::LG_WALL); diagonals_removed++; }
                    diagonal_removed = true;
                }
            }
        }
    } while (diagonal_removed);
    if (diagonals_removed && AREA_GEN_DEBUG_MESSAGES) guru->log("Removed " + std::to_string(diagonals_removed) + " diagonal floor tiles.");

    // Find locations for the up and down stairs.
    auto valid_stairs_position = [this](int x, int y) -> bool {
        if (area_->tile(x, y)->id() != TileID::LG_FLOOR) return false;
        if (neighbours(x, y, TileID::LG_WALL, false) != 2) return false;
        if (neighbours(x, y, TileID::LG_FLOOR, false) != 2) return false;
        if (area_->tile(x - 1, y)->id() == TileID::LG_WALL && area_->tile(x + 1, y)->id() == TileID::LG_FLOOR) return true;
        if (area_->tile(x - 1, y)->id() == TileID::LG_FLOOR && area_->tile(x + 1, y)->id() == TileID::LG_WALL) return true;
        return false;
    };

    for (int i = 0; i < 2; i++)
    {
        const bool up = (i == 0);
        int current_room = (i == 0 ? 0 : rooms_.size() - 1);
        int room_dir = (i == 0 ? 1 : -1);
        int end_room = (i == 0 ? rooms_.size() - 1 : 0);

        do
        {
            std::vector<std::pair<int, int>> possible_stair_locations;
            for (int x = 0; x < std::get<2>(rooms_.at(current_room)); x++)
            {
                for (int y = 0; y < std::get<3>(rooms_.at(current_room)); y++)
                {
                    const int px = std::get<0>(rooms_.at(current_room)) + x;
                    const int py = std::get<1>(rooms_.at(current_room)) + y;
                    if (valid_stairs_position(px, py)) possible_stair_locations.push_back(std::pair<int, int>(px, py));
                }
            }
            if (possible_stair_locations.size())
            {
                int choice = Random::rng(possible_stair_locations.size()) - 1;
                area_->set_tile(possible_stair_locations.at(choice).first, possible_stair_locations.at(choice).second, up ? TileID::STAIRS_UP :
                    TileID::STAIRS_DOWN);
                if (up) stairs_up_room_ = current_room;
                break;
            }
            else current_room += room_dir;
        } while (current_room != end_room);
        if (current_room == end_room)
        {
            if (AREA_GEN_DEBUG_MESSAGES) guru->log("ABORTING: Could not find viable stair locations!");
            void_map();
            generate();
            return;
        }
    }
    if (AREA_GEN_DEBUG_MESSAGES) guru->log("Stairs placed.");

    for (unsigned int i = 0; i < rooms_.size(); i++)
        decorate_room(i);
    if (AREA_GEN_DEBUG_MESSAGES) guru->log("Decoration complete.");

    // Bake the tiles.
    for (int x = 0; x < area_->width(); x++)
    {
        for (int y = 0; y < area_->height(); y++)
        {
            auto the_tile = area_->tile(x, y);
            TileID chosen_tile = TileID::VOID_TILE;
            switch (the_tile->id())
            {
                case TileID::LG_FLOOR: chosen_tile = TileID::FLOOR_STONE; break;
                case TileID::LG_WALL: chosen_tile = TileID::WALL_STONE; break;
                case TileID::LG_DOOR_CANDIDATE: chosen_tile = TileID::DOOR_WOOD; break;
                case TileID::STAIRS_UP: case TileID::STAIRS_DOWN: chosen_tile = the_tile->id(); break;
                case TileID::DRUJ_TOMB:
                {
                    chosen_tile = TileID::DRUJ_TOMB;
                    auto new_mob = CodexMonster::generate(MonsterID::DRUJ_WALKER);
                    new_mob->set_pos(x, y);
                    area_->entities()->push_back(new_mob);
                    break;
                }
                default: break;
            }
            if (chosen_tile == TileID::VOID_TILE)
            {
                if (AREA_GEN_DEBUG_MESSAGES) guru->log("ABORTING: Invalid tile detected during baking process!");
                void_map();
                generate();
                return;
            }
            if (the_tile->id() != chosen_tile) area_->set_tile(x, y, chosen_tile);
        }
    }
}

// Checks how many neighbouring tiles are the specified type
int DungeonGenerator::neighbours(int x, int y, TileID type, bool diagonals, int range)
{
    int count = 0;
    for (int cx = x - range; cx <= x + range; cx++)
    {
        for (int cy = y - range; cy <= y + range; cy++)
        {
            if (cx < 0 || cy < 0 || cx >= area_->width() || cy >= area_->height() || (cx == x && cy == y)) continue;
            if (!diagonals && std::abs(cx - x) == std::abs(cy - y)) continue;
            if (area_->tile(cx, cy)->id() == type) count++;
        }
    }
    return count;
}

// Attempts to paste a room at the given coordinates.
bool DungeonGenerator::paste_room(std::shared_ptr<Room> room, int x, int y)
{
    if (x < 0 || y < 0 || x + room->actual_width() >= area_->width() - 1 || y + room->actual_height() >= area_->height() - 1) return false;

    // Do verification on anything but the first room, to make sure it fits.
    if (!first_room_)
    {
        int door_candidates_match = 0;
        for (int rx = 0; rx < room->actual_width(); rx++)
        {
            for (int ry = 0; ry < room->actual_height(); ry++)
            {
                auto room_tile = room->tile(rx, ry);
                auto map_tile = area_->tile(x + rx, y + ry);
                if (room_tile->id() == TileID::VOID_TILE || map_tile->id() == TileID::VOID_TILE) continue;  // Void tiles can overlap void tiles.
                if (room_tile->id() == TileID::LG_DOOR_CANDIDATE && map_tile->id() == TileID::LG_DOOR_CANDIDATE) door_candidates_match++;
                else if (room_tile->id() != TileID::VOID_TILE && map_tile->id() != TileID::VOID_TILE) return false; // No overlaps, except door candidates.
            }
        }
        if (!door_candidates_match) return false;   // At least one door candidate must align with another.
    }
    for (int rx = 0; rx < room->actual_width(); rx++)
        for (int ry = 0; ry < room->actual_height(); ry++)
            if (room->tile(rx, ry)->id() != TileID::VOID_TILE) area_->set_tile(x + rx, y + ry, room->tile(rx, ry)->id());
    return true;
}

// Voids (empties) this entire map.
void DungeonGenerator::void_map()
{
    area_->void_area();
    first_room_ = true;
    active_room_ = -1;
    rooms_.clear();
}

// Creates a new Room.
Room::Room(DungeonGenerator *dg, int w, int h) : actual_height_(h), actual_width_(w), height_(h), dungeon_gen_(dg), width_(w)
{ tiles_ = new Tile[w * h](); }

// Cleans up memory used.
Room::~Room() { delete[] tiles_; }

// Returns the actual height of this Room, after it's been shunted into a corner.
int Room::actual_height() const { return actual_height_; }

// Returns the actual width of this Room, after it's been shunted into a corner.
int Room::actual_width() const { return actual_width_; }

// Applies a door candidate tile to a side of a room.
void Room::apply_door_candidate(int sx, int sy, int dx, int dy)
{
    std::vector<std::pair<int, int>> viable_options;
    while (sx >= 0 && sy >= 0 && sx < width_ && sy < height_)
    {
        if (std::abs(dx))
        {
            for (int y = 0; y < height_; y++)
                if (neighbours(sx, y, TileID::LG_FLOOR, false) == 1 && neighbours(sx, y, TileID::LG_FLOOR, true) == 3 && neighbours(sx, y,
                    TileID::LG_DOOR_CANDIDATE, true) == 0) viable_options.push_back(std::pair<int, int>(sx, y));
        }
        else
        {
            for (int x = 0; x < width_; x++)
                if (neighbours(x, sy, TileID::LG_FLOOR, false) == 1 && neighbours(x, sy, TileID::LG_FLOOR, true) == 3 && neighbours(x, sy,
                    TileID::LG_DOOR_CANDIDATE, true) == 0) viable_options.push_back(std::pair<int, int>(x, sy));
        }
        if (viable_options.size()) break;
        sx += dx;
        sy += dy;
    }
    if (viable_options.size())
    {
        auto target = viable_options.at(Random::rng(viable_options.size()) - 1);
        set_tile(target.first, target.second, TileID::LG_DOOR_CANDIDATE);
    }
}

// Digs floor space into the room.
void Room::dig(int x1, int y1, int x2, int y2)
{
    if (x2 < x1) std::swap(x1, x2);
    if (y2 < y1) std::swap(y1, y2);

    for (int tx = x1; tx <= x2; tx++)
        for (int ty = y1; ty <= y2; ty++)
            set_tile(tx, ty, TileID::LG_FLOOR_CANDIDATE);
}

// Flood-fills the room, to check all floor tiles are accessible.
bool Room::flood_check()
{
    int floor_tiles = 0;
    int tiles_found = 0;
    int start_x = -1, start_y = 1;
    for (int x = 0; x < width_; x++)
    {
        for (int y = 0; y < height_; y++)
        {
            auto the_tile = tile(x, y);
            if (the_tile->id() == TileID::LG_FLOOR_CANDIDATE)
            {
                floor_tiles++;
                if (start_x == -1)
                {
                    start_x = x;
                    start_y = y;
                }
            }
        }
    }
    if (floor_tiles < DUNGEON_ROOM_FLOOR_TILES_MIN || start_x < 0 || start_y < 0) return false;

    std::function<void(int, int)> flood_fill;
    flood_fill = [this, &tiles_found, &flood_fill](int sx, int sy) {
        auto the_tile = tile(sx, sy);
        if (the_tile->id() != TileID::LG_FLOOR_CANDIDATE) return;
        set_tile(sx, sy, TileID::LG_FLOOR);
        tiles_found++;

        for (int x = sx - 1; x <= sx + 1; x++)
        {
            for (int y = sy - 1; y <= sy + 1; y++)
            {
                if (x < 0 || y < 0 || x >= width_ || y >= height_ || (x == sx && y == sy)) continue;
                the_tile = tile(x, y);
                if (the_tile->id() == TileID::LG_FLOOR_CANDIDATE) flood_fill(x, y);
            }
        }
    };

    flood_fill(start_x, start_y);

    if (tiles_found != floor_tiles) return false;
    else return true;
}

// Generates the room; returns false if the end result is not viable.
bool Room::generate(bool first)
{
    dungeon_gen_->active_room_ = -1;
    bool viable = false;
    int tries = 0;
    do
    {
        generate_type_a();
        viable = flood_check();
        if (!viable)
        {
            tries++;
            void_room();
        }
    } while (!viable);
    int door_candidates = (first ? 15 : Random::rng(15));
    if ((door_candidates & 1) == 1) apply_door_candidate(0, 0, 1, 0);
    if ((door_candidates & 2) == 2) apply_door_candidate(width_ - 1, 0, -1, 0);
    if ((door_candidates & 4) == 4) apply_door_candidate(0, 0, 0, 1);
    if ((door_candidates & 8) == 8) apply_door_candidate(0, height_ - 1, 0, -1);
    if (AREA_GEN_DEBUG_MESSAGES) core()->guru()->log("Room generated (" + std::to_string(tries) + " retries)");

    while (top_row_blank()) shunt_up();
    while (left_row_blank()) shunt_left();

    // Count the number of blank columns.
    for (int x = 0; x < width_; x++)
    {
        bool all_blank = true;
        for (int y = 0; y < height_; y++)
        {
            if (tile(x, y)->id() != TileID::VOID_TILE)
            {
                all_blank = false;
                break;
            }
        }
        if (all_blank) actual_width_--;
    }

    // Count the number of blank rows.
    for (int y = 0; y < height_; y++)
    {
        bool all_blank = true;
        for (int x = 0; x < width_; x++)
        {
            if (tile(x, y)->id() != TileID::VOID_TILE)
            {
                all_blank = false;
                break;
            }
        }
        if (all_blank) actual_height_--;
    }

    return true;
}

// Room generation type A: overlay two random rectangles.
void Room::generate_type_a()
{
    for (int i = 0; i < 2; i++)
    {
        int x1, y1, x2, y2;
        do
        {
            x1 = Random::rng(width_ - 2);
            x2 = Random::rng(width_ - 2);
        } while (std::abs(x1 - x2) <= 2);
        do
        {
            y1 = Random::rng(height_ - 2);
            y2 = Random::rng(height_ - 2);
        } while (std::abs(y1 - y2) <= 2);
        dig(x1, y1, x2, y2);
    }
}

// Returns true if the left row is empty.
bool Room::left_row_blank()
{
    for (int y = 0; y < height_; y++)
        if (tile(0, y)->id() != TileID::VOID_TILE) return false;
    return true;
}

// Checks how many neighbouring tiles are the specified type
int Room::neighbours(int x, int y, TileID type, bool diagonals)
{
    int count = 0;
    for (int cx = x - 1; cx <= x + 1; cx++)
    {
        for (int cy = y - 1; cy <= y + 1; cy++)
        {
            if (cx < 0 || cy < 0 || cx >= width_ || cy >= height_ || (cx == x && cy == y)) continue;
            if (!diagonals && std::abs(cx - x) == std::abs(cy - y)) continue;
            if (tile(cx, cy)->id() == type) count++;
        }
    }
    return count;
}

// Sets a Tile in this Room.
void Room::set_tile(int x, int y, TileID type)
{
    if (x < 0 || y < 0 || x >= width_ || y >= height_) throw std::runtime_error("Invalid map tile requested!");
    CodexTile::generate(&tiles_[x + (y * width_)], type);
}

// Shunts the room left by one column.
void Room::shunt_left()
{
    for (int x = 0; x < width_; x++)
        for (int y = 0; y < height_; y++)
            if (x == width_ - 1) set_tile(x, y, TileID::VOID_TILE); else set_tile(x, y, tile(x + 1, y)->id());
}

// Shunts the room up by one row.
void Room::shunt_up()
{
    for (int x = 0; x < width_; x++)
        for (int y = 0; y < height_; y++)
            if (y == height_ - 1) set_tile(x, y, TileID::VOID_TILE); else set_tile(x, y, tile(x, y + 1)->id());
}

Tile* Room::tile(int x, int y)
{
    if (x < 0 || y < 0 || x >= width_ || y >= height_) throw std::runtime_error("Invalid map tile requested!");
    return &tiles_[x + (y * width_)];
}

// Returns true if the top row is empty.
bool Room::top_row_blank()
{
    for (int x = 0; x < width_; x++)
        if (tile(x, 0)->id() != TileID::VOID_TILE) return false;
    return true;
}

// Voids (empties) this room.
void Room::void_room()
{
    for (int i = 0; i < width_ * height_; i++)
        CodexTile::generate(&tiles_[i], TileID::VOID_TILE);
}

}   // namespace invictus
