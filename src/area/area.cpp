// area/area.cpp -- The Area class, which defines an area in the game world that the player can move around in.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <algorithm>
#include <cmath>

#include "area/area.hpp"
#include "area/shadowcast.hpp"
#include "area/tile.hpp"
#include "codex/codex-tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/ui.hpp"
#include "util/bresenham.hpp"


namespace invictus
{

// Constructor, creates a new empty Area.
Area::Area(int width, int height) : cleanup_done_(false), needs_fov_recalc_(true), offset_x_(0), offset_y_(0), size_x_(width), size_y_(height)
{
    if (width < 0 || height < 0) core()->guru()->halt("Invalid Area size", width, height);
    tiles_ = new Tile[width * height]();
    visible_ = new bool[width * height];
    std::fill_n(visible_, width * height, false);
    tile_memory_ = new char[width * height];
    std::fill_n(tile_memory_, width * height, ' ');
}

// Destructor, cleans up memory.
Area::~Area() { cleanup(); }

// Checks if the a Mobile can walk onto a specified tile.
bool Area::can_walk(int x, int y)
{
    if (x < 0 || y < 0 || x >= size_x_ || y >= size_y_) return false;
    if (tile(x, y)->tag(TileTag::BlocksMovement)) return false;
    //for (auto actor : *actors())
        //if (actor->blocks_tile(x, y)) return false;
    return true;
}

// Cleans up memory used.
void Area::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    if (tiles_)
    {
        delete[] tiles_;
        tiles_ = nullptr;
    }
    if (visible_)
    {
        delete[] visible_;
        visible_ = nullptr;
    }
    if (tile_memory_)
    {
        delete[] tile_memory_;
        tile_memory_ = nullptr;
    }
}

// Returns the vector of Entities within this Area.
std::vector<std::shared_ptr<Entity>>* Area::entities() { return &entities_; }

// Finds an Entity's index on the list of Entities.
uint32_t Area::entity_index(std::shared_ptr<Entity> entity)
{ return std::distance(entities()->begin(), std::find(entities()->begin(), entities()->end(), entity)); }

// Finds a tile with the specified tag.
std::pair<int, int> Area::find_tile_tag(TileTag tag)
{
    for (int x = 0; x < size_x_; x++)
        for (int y = 0; y < size_y_; y++)
            if (tiles_[x + (y * size_x_)].tag(tag)) return {x, y};
    return {0, 0};
}

// Checks the distance between two points, returns -1 if something opaque is in the way.
float Area::fov_distance(int x, int y, int x2, int y2)
{
    const int x1 = x, y1 = y;
    auto bresenham = std::make_unique<BresenhamLine>(x, y, x2, y2);
    do
    {
        if (tile(x, y)->tag(TileTag::BlocksLight) && (x != x2 || y != y2)) return -1;
        std::pair<int, int> result = bresenham->step();
        x = result.first;
        y = result.second;
    } while(x != x2 || y != y2);

    return grid_distance(x1, y1, x2, y2);
}

// Calculates the distance between two points, regardless of line of sight.
float Area::grid_distance(int x, int y, int x2, int y2) const
{
    int xm = x2 - x, ym = y2 - y;
    return sqrtf((xm * xm) + (ym * ym));
}

// Read-only access to the Area's height.
uint16_t Area::height() const { return size_y_; }

// Checks if a given Tile is within the player's field of view.
uint8_t Area::is_in_fov(int x, int y)
{
    if (x < 0 || y < 0 || x >= width() || y >= height()) core()->guru()->halt("Invalid map tile requested!", x, y);
    auto player = core()->game()->player();
    if (player->is_at(x, y)) return true;
    return visible_[x + (y * size_x_)];
}

// Checks if a given Tile is blocking light.
bool Area::is_opaque(int x, int y)
{
    if (tile(x, y)->tag(TileTag::BlocksLight)) return true;
    return false;
}

// Marks the Area as needing a FoV recalc.
void Area::need_fov_recalc() { needs_fov_recalc_ = true; }

// Retrieves the view offset on the X axis.
int Area::offset_x() const { return offset_x_; }

// Retrieves the view offset on the Y axis.
int Area::offset_y() const { return offset_y_; }

// Recalculates the player's field of view.
void Area::recalc_fov()
{
    if (!needs_fov_recalc_) return;

    std::fill_n(visible_, size_x_ * size_y_, false);
    auto game = core()->game();
    auto player = game->player();
    Shadowcast::calc_fov(this, player->x(), player->y(), player->fov_radius());

    needs_fov_recalc_ = false;
}

// Renders this Area on the screen.
void Area::render()
{
    auto terminal = core()->terminal();
    auto player = core()->game()->player();
    auto dungeon_view = core()->game()->ui()->dungeon_view();

    recalc_fov();   // Recalculates the player's field of view and lighting, if needed.

    int visible_x = dungeon_view->get_width(), visible_y = dungeon_view->get_height();
    offset_x_ = offset_y_ = 0;

    if (size_x_ >= visible_x)
    {
        offset_x_ = player->x() - (visible_x / 2);
        if (offset_x_ < 0) offset_x_ = 0;
        else if (offset_x_ + visible_x > size_x_) offset_x_ = size_x_ - visible_x;
    }
    else if (size_x_ < visible_x) offset_x_ = -((visible_x - size_x_) / 2);
    if (size_y_ >= visible_y)
    {
        offset_y_ = player->y() - (visible_y / 2);
        if (offset_y_ < 0) offset_y_ = 0;
        else if (offset_y_ + visible_y > size_y_) offset_y_ = size_y_ - visible_y;
    }
    else if (size_y_ < visible_y) offset_y_ = -((visible_y - size_y_) / 2);

    for (int x = 0; x < size_x_; x++)
    {
        int ox = x - offset_x_;
        if (ox < 0 || ox >= visible_x) continue;
        for (int y = 0; y < size_y_; y++)
        {
            int oy = y - offset_y_;
            if (oy < 0 || oy >= visible_y) continue;

            auto the_tile = tile(x, y);
            bool is_visible = is_in_fov(x, y);
            char tile_memory = tile_memory_[x + (y * size_x_)];
            bool is_explored = (tile_memory != ' ');
            if (!is_visible && !is_explored) continue;

            Colour colour = the_tile->colour();
            char ascii = the_tile->ascii();
            if (!is_visible)
            {
                colour = Colour::BLUE;
                ascii = tile_memory;
            }
            terminal->put(ascii, ox, oy, colour, 0, dungeon_view);
        }
    }

    // We'll render Actors in several passes, to ensure more important things are on top.

    // First pass: Corpses.

    // Second pass: Items.

    // Third pass: Mobiles.

    // Fourth pass: the player. No need for off-screen checks here, the player should always be on the screen.
    terminal->put(player->ascii(), player->x() - offset_x_, player->y() - offset_y_, player->colour(), 0, dungeon_view);
}

// Sets a Tile to something else.
void Area::set_tile(int x, int y, TileID tile_id)
{
    if (x < 0 || y < 0 || x >= width() || y >= height()) core()->guru()->halt("Invalid map tile requested!", x, y);
    CodexTile::generate(&tiles_[x + (y * size_x_)], tile_id);
}

// Sets a specified Tile as visible.
void Area::set_visible(int x, int y)
{
    if (x < 0 || y < 0 || x >= width() || y >= height()) core()->guru()->halt("Invalid map tile requested!", x, y);
    visible_[x + (y * size_x_)] = true;
    tile_memory_[x + (y * size_x_)] = tile(x, y)->ascii();
}

// Gets a specified Tile.
Tile* Area::tile(int x, int y)
{
    if (x < 0 || y < 0 || x >= size_x_ || y >= size_y_) core()->guru()->halt("Invalid map tile requested!", x, y);
    return &tiles_[x + (y * size_x_)];
}

// Retrieves the player's memory of a given Tile.
char Area::tile_memory(int x, int y)
{
    if (x < 0 || y < 0 || x >= size_x_ || y >= size_y_) core()->guru()->halt("Invalid map tile requested!", x, y);
    return tile_memory_[x + (y * size_x_)];
}

// Erases this entire Area.
void Area::void_area()
{
    for (int i = 0; i < size_x_ * size_y_; i++)
        CodexTile::generate(&tiles_[i], TileID::VOID_TILE);
    entities_.clear();
    entities_.push_back(core()->game()->player());
    std::fill_n(visible_, size_x_ * size_y_, false);
    std::fill_n(tile_memory_, size_x_ * size_y_, ' ');
}

// Read-only access to the Area's width.
uint16_t Area::width() const { return size_x_; }

}   // namespace invictus
