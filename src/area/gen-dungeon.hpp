// area/gen-dungeon.hpp -- The cool procedural dungeon area generator.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef AREA_GEN_DUNGEON_HPP_
#define AREA_GEN_DUNGEON_HPP_

#include <memory>
#include <tuple>
#include <vector>


namespace invictus
{

enum class TileID : uint16_t;   // defined in factory/factory-tile.hpp

class Area; // defined in area/area.hpp
class Room; // defined below
class Tile; // defined in area/tile.hpp


class DungeonGenerator
{
public:
            DungeonGenerator(std::shared_ptr<Area> area_to_gen);    // Prepares an Area for procedural generation.
    void    generate(); // Generates the new map!

private:
    void    decorate_room(unsigned int room_id);    // Decorates a specified room.
    bool    decorate_room_druj_tombs(unsigned int room_id); // Room decoration: druj burial tombs.
    void    find_wall(int *x, int *y, int dx, int dy, unsigned int room_id);    // Finds the first wall in the room, going in the specified direction (dx/dy).
    void    get_internal_room_size(unsigned int room_id, int *x, int *y, int *w, int *h);   // Gets the internal coordinates of a room, not counting doors.
    int     neighbours(int x, int y, TileID type, bool diagonals = true, int range = 1);    // Checks how many neighbouring tiles are the specified type.
    bool    paste_room(std::shared_ptr<Room> room, int x, int y);   // Attempts to paste a room at the given coordinates.
    void    void_map(); // Voids (empties) this entire map.

    int         active_room_;       // The current room being modified.
    std::shared_ptr<Area>   area_;  // The Area being created by this dungeon generator.
    bool        first_room_;        // The first room requires no links, obviously.
    std::vector<std::tuple<int, int, int, int>> rooms_; // The X,Y coordinates, width and height of each room.
    int         stairs_up_room_;    // Which room are the upward stairs located in?

    friend class Room;
};


class Room
{
public:
    Room(DungeonGenerator *dg, int w, int h);   // Creates a new Room.
    ~Room();    // Cleans up memory used.

private:
    int     actual_height() const;  // Returns the actual height of this Room, after it's been shunted into a corner.
    int     actual_width() const;   // Returns the actual width of this Room, after it's been shunted into a corner.
    void    apply_door_candidate(int sx, int sy, int dx, int dy);   // Applies a door candidate tile to a side of a room.
    bool    flood_check();          // Flood-fills the room, to check all floor tiles are accessible.
    void    dig(int x1, int y1, int x2, int y2);    // Digs floor space into the room.
    bool    generate(bool first_room = false);      // Generates the room; returns false if the end result is not viable.
    void    generate_type_a();  // Room generation type A: overlay two random rectangles.
    bool    left_row_blank();   // Returns true if the left row is empty.
    int     neighbours(int x, int y, TileID type, bool diagonals = true);   // Checks how many neighbouring tiles are the specified type.
    void    set_tile(int x, int y, TileID type);    // Sets a Tile in this Room.
    void    shunt_left();       // Shunts the room left by one column.
    void    shunt_up();         // Shunts the room up by one row.
    Tile*   tile(int x, int y); // Gets a specified tile.
    bool    top_row_blank();    // Returns true if the top row is empty.
    void    void_room();        // Voids (empties) this room.

    int                 actual_height_, actual_width_;  // The width/height of this room after it's been shunted into a corner.
    int                 height_;        // The height of this room.
    DungeonGenerator*   dungeon_gen_;   // The DungeonGenerator that spawned this Room.
    Tile*               tiles_; // The Tiles that compose this Room.
    int                 width_;         // The width of this room.

    friend class DungeonGenerator;
};

}       // namespace invictus
#endif  // AREA_GEN_DUNGEON_HPP_
