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
    void    generate(bool with_actors = false); // Generates the new map!

private:
    static constexpr bool   DEBUG_GENERATION_MESSAGES = false;  // Enable debug dungeon generation messages in the log.
    static constexpr int    MAP_CORNER_SMOOTHING =  5;  // The lower this number, the more rooms will have smooth edges rather than being square.
                                                        // Set to 1 for full smoothing, 0 for none.
    static constexpr int    MAP_MAX_WALKABLE =      50; // The maximum % of the map that needs to be walkable (i.e. floor) to be considered viable.
                                                        // Too much of this can be bad, as it leads to cluttered maps.
    static constexpr int    MAP_MIN_WALKABLE =      20; // The minimum % of the map that needs to be walkable (i.e. floor) to be considered viable.
                                                        // Too little of this can be bad, as it leads to very small maps.
    static constexpr int    ROOM_GEN_RETRIES =      10; // How many new rooms are generated after a room was unable to be inserted in the map?
                                                        // The higher this number, the better quality maps will be generated, but it'll be much slower.
    static constexpr int    ROOM_SIZE_MAX =         12; // The largest room allowed by the map generator.
    static constexpr int    ROOM_SIZE_MIN =         6;  // The smallest room allowed by the map generator.
    static constexpr int    WALL_TOMB_CHANCE =      5;  // 1 in X chance of a viable wall location being dug out into an alcove for a burial tomb.

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
    static constexpr int    ROOM_FLOOR_TILES_MIN =  10; // Generated rooms must have at least this many floor tiles.

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
