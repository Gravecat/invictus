// area/gore.cpp -- Handles splashes of blood and other viscera from combat.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/gore.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/gore.hpp"
#include "util/random.hpp"

#include "core/guru.hpp"


namespace invictus
{

// Performs a single splatter.
void Gore::do_splash(int x, int y)
{
    const auto area = core()->game()->area();
    if (x < 0 || y < 0 || x >= area->width() || y >= area->height()) return;

    Tile* tile = area->tile(x, y);
    if (tile->tag(TileTag::BlocksMovement)) return;
    int level = gore_level(x, y);
    unsigned int spread_chance = (level - 1) * GORE_SPREAD_CHANCE_MULTI;
    if (level == 1) spread_chance = GORE_SPREAD_CHANCE_LOW;

    int current_gore = gore_level(x, y);
    int new_gore_level = current_gore + 1;
    if (new_gore_level >= 7)
    {
        int roll = Random::rng(10);
        if (roll <= 5) new_gore_level = 6;
        else if (roll <= 8) new_gore_level = 5;
        else if (roll == 9) new_gore_level = 4;
        else new_gore_level = 3;
    }
    set_gore(x, y, new_gore_level);

    if (Random::rng(100) <= spread_chance)
    {
        int dx = (Random::rng(2) == 1 ? -1 : 1);
        int dy = static_cast<int>(Random::rng(3)) - 2;
        do_splash(x + dx, y + dy);
    }
}

// Determine the gore level of a given tile.
int Gore::gore_level(int x, int y)
{
    const Tile* tile = core()->game()->area()->tile(x, y);
    if (!tile->tag(TileTag::Bloodied)) return 0;

    switch(tile->ascii())
    {
        case ASCII_GORE_1: return 1;
        case ASCII_GORE_2: return 2;
        case ASCII_GORE_3: return 3;
        case ASCII_GORE_4: return 4;
        case ASCII_GORE_5: return 5;
        case ASCII_GORE_6A: case ASCII_GORE_6B: return 6;
        default: return 1;
    }

    return 0;
}

// Sets a tile to a given gore level.
void Gore::set_gore(int x, int y, int level)
{
    Tile* tile = core()->game()->area()->tile(x, y);
    if (tile->tag(TileTag::Immutable)) return;
    tile->set_tag(TileTag::Bloodied);
    Colour col = Colour::RED;
    switch(level)
    {
        case 1: tile->set_scars(ASCII_GORE_1, col); break;
        case 2: tile->set_scars(ASCII_GORE_2, col); break;
        case 3: tile->set_scars(ASCII_GORE_3, col); break;
        case 4: tile->set_scars(ASCII_GORE_4, col); break;
        case 5: tile->set_scars(ASCII_GORE_5, col); break;
        default: if (Random::rng(GORE_ALT_CHAR_CHANCE) == 1) tile->set_scars(ASCII_GORE_6B, col); else tile->set_scars(ASCII_GORE_6A, col); break;
    }
}

// Splashes blood and gore on a given tile.
void Gore::splash(int x, int y, int intensity)
{
    if (intensity < 5 && Random::rng(3) == 1) intensity += Random::rng(3);  // Below level 5, there's a 1 in 3 chance to escalate by 1-3 levels.
    else if (intensity < 10) intensity += Random::rng(0, 5);    // From levels 5-9, escalate between 0-5 levels.
    else intensity += Random::rng(0, 10);   // At level 10+, escalate from 0-10 levels.

    for (int i = 0; i < intensity; i++)
        do_splash(x, y);
    core()->game()->area()->need_fov_recalc();
}

}   // namespace invictus
