// area/area.hpp -- The Area class, which defines an area in the game world that the player can move around in.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef AREA_AREA_HPP_
#define AREA_AREA_HPP_

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>


namespace invictus
{

enum class TileID : uint16_t;   // defined in factory/factory-tile.hpp
enum class TileTag : uint16_t;  // defined in area/tile.hpp

class Entity;   // defined in entity/entity.hpp
class Tile;     // defined in area/tile.hpp


class Area
{
public:
                Area(int width, int height);    // Constructor, creates a new empty Area.
                ~Area();    // Destructor, cleans up memory.
    bool        can_walk(int x, int y); // Checks if the a Mobile can walk onto a specified tile.
    void        cleanup();  // Cleans up memory used.
    std::vector<std::shared_ptr<Entity>>*   entities(); // Returns the vector of Entities within this Area.
    uint32_t    entity_index(std::shared_ptr<Entity> entity);   // Finds an Entity's index on the list of Entities.
    std::pair<int, int> find_tile_tag(TileTag tag);             // Finds a tile with the specified tag.
    float       fov_distance(int x, int y, int x2, int y2);     // Checks the distance between two points, returns -1 if something opaque is in the way.
    float       grid_distance(int x, int y, int x2, int y2) const;  // Calculates the distance between two points, regardless of line of sight.
    uint16_t    height() const;     // Read-only access to the Area's height.
    uint8_t     is_in_fov(int x, int y);    // Checks if a given Tile is within the player's field of view.
    bool        is_opaque(int x, int y);    // Checks if a given Tile is blocking light.
    void        need_fov_recalc();  // Marks the Area as needing a FoV recalc.
    int         offset_x() const;   // Retrieves the view offset on the X axis.
    int         offset_y() const;   // Retrieves the view offset on the Y axis.
    void        render();           // Renders this Area on the screen.
    void        set_tile(int x, int y, TileID tile_id); // Sets a Tile to something else.
    void        set_visible(int x, int y);  // Sets a specified Tile as visible.
    Tile*       tile(int x, int y); // Gets a specified Tile.
    char        tile_memory(int x, int y);  // Retrieves the player's memory of a given Tile.
    void        void_area();        // Erases this entire Area.
    uint16_t    width() const;      // Read-only access to the Area's width.

private:
    void        recalc_fov();       // Recalculates the player's field of view.

    bool        cleanup_done_;      // Has the cleanup routine already run once?
    std::vector<std::shared_ptr<Entity>>    entities_;  // The Entities within this Area.
    bool        needs_fov_recalc_;  // Set this to TRUE to force a field-of-view recalculation on the next render.
    int         offset_x_, offset_y_;   // Screen rendering offsets.
    uint16_t    size_x_, size_y_;   // The X/Y dimensions of this Area.
    char*       tile_memory_;   // The player's memory of previously-seen Tiles.
    Tile*       tiles_;     // An array of Tiles that make up this Area.
    bool*       visible_;   // Which tiles are currently visible by the player.
};

}       // namespace invictus
#endif  // AREA_AREA_HPP_
