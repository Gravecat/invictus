// codex/codex-tile.cpp -- Factory class, generating preset types of Tiles that make up the game world's structure.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/tile.hpp"
#include "codex/codex-tile.hpp"
#include "core/core.hpp"
#include "core/guru.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"


namespace invictus
{

// Generates a preset Tile.
void CodexTile::generate(Tile* tile, TileID id)
{
    if (!tile) core()->guru()->halt("Attempt to generate nullptr tile!");
    tile->tags_.clear();
    tile->id_ = id;

    switch(id)
    {
        case TileID::VOID_TILE: // This shouldn't really be used for anything, it's just a filler "nothing here" tile.
            tile->name_ = "void";
            tile->ascii_ = ASCII_NOTHING;
            tile->colour_ = Colour::BLACK;
            tile->set_tag(TileTag::BlocksMovement, false);
            break;

        case TileID::FLOOR_STONE:   // A basic and generic floor tile.
            tile->name_ = "stone floor";
            tile->ascii_ = ASCII_GROUND;
            tile->colour_ = Colour::WHITE;
            break;

        case TileID::WALL_STONE:    // A basic and generic wall tile.
            tile->name_ = "stone wall";
            tile->ascii_ = ASCII_WALL;
            tile->colour_ = Colour::WHITE;
            tile->set_tags({TileTag::BlocksLight, TileTag::BlocksMovement}, false);
            break;

        case TileID::STAIRS_DOWN:   // Stairs leading down to the dungeon level below.
            tile->name_ = "stairs down";
            tile->ascii_ = ASCII_STAIRS_DOWN;
            tile->colour_ = Colour::WHITE_BOLD;
            tile->set_tags({TileTag::Immutable, TileTag::StairsDown}, false);
            break;

        case TileID::STAIRS_UP: // Stairs leading up to the dungeon level (or surface world) above.
            tile->name_ = "stairs up";
            tile->ascii_ = ASCII_STAIRS_UP;
            tile->colour_ = Colour::WHITE_BOLD;
            tile->set_tags({TileTag::Immutable, TileTag::StairsUp}, false);
            break;

        case TileID::WALL_BEDROCK:  // Indestructible walls surrounding the map.
            tile->name_ = "bedrock wall";
            tile->ascii_ = ASCII_WALL;
            tile->colour_ = Colour::BLACK_BOLD;
            tile->set_tags({TileTag::Immutable, TileTag::BlocksMovement, TileTag::BlocksLight}, false);
            break;

        case TileID::LG_FLOOR:  // Dungeon generation: will become walkable floor.
            tile->name_ = "unfinished floor";
            tile->ascii_ = ASCII_GROUND;
            tile->colour_ = Colour::BLACK_BOLD;
            break;

        case TileID::LG_WALL:   // Dungeon generation: will become a solid wall.
            tile->name_ = "unfinished wall";
            tile->ascii_ = ASCII_WALL;
            tile->colour_ = Colour::BLACK_BOLD;
            tile->set_tags({TileTag::BlocksLight, TileTag::BlocksMovement}, false);
            break;

        case TileID::LG_DOOR_CANDIDATE: // Dungeon generation: may become a door.
            tile->name_ = "door candidate";
            tile->ascii_ = ASCII_DOOR_CLOSED;
            tile->colour_ = Colour::YELLOW;
            tile->set_tags({TileTag::BlocksLight, TileTag::BlocksMovement}, false);
            break;

        case TileID::LG_FLOOR_CANDIDATE:    // Dungeon generation: may become floor.
            tile->name_ = "floor candidate";
            tile->ascii_ = ASCII_GROUND;
            tile->colour_ = Colour::BLACK_BOLD;
            break;

        case TileID::DRUJ_TOMB: // Druj tombs, which spawn undead.
            tile->name_ = "druj tomb";
            tile->ascii_ = ASCII_TOMB;
            tile->colour_ = Colour::BLACK_BOLD;
            tile->set_tags({TileTag::BlocksLight, TileTag::BlocksMovement}, false);
            break;

        case TileID::DOOR_WOOD: // Wooden door
            tile->name_ = "wooden door";
            tile->ascii_ = ASCII_DOOR_CLOSED;
            tile->colour_ = Colour::YELLOW;
            tile->set_tags({TileTag::BlocksLight, TileTag::Openable}, false);
            break;
    }
}

}   // namespace invictus
