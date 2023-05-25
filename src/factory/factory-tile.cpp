// factory/factory-tile.cpp -- Factory class, generating preset types of Tiles that make up the game world's structure.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/guru.hpp"
#include "factory/factory-tile.hpp"
#include "terminal/terminal-shared-defs.hpp"


namespace invictus
{

// Generates a preset Tile.
void FactoryTile::generate(std::shared_ptr<Tile> tile, TileID id)
{
    if (!tile) core()->guru()->halt("Attempt to generate nullptr tile!");
    tile->tags_.clear();

    switch(id)
    {
        case TileID::VOID_TILE: // This shouldn't really be used for anything, it's just a filler "nothing here" tile.
            tile->name_ = "void";
            tile->ascii_ = ' ';
            tile->colour_ = Colour::BLACK;
            tile->set_tag(TileTag::BlocksMovement);
            break;

        case TileID::FLOOR_STONE:   // A basic and generic floor tile.
            tile->name_ = "stone floor";
            tile->ascii_ = '.';
            tile->colour_ = Colour::WHITE;
            break;

        case TileID::WALL_STONE:    // A basic and generic wall tile.
            tile->name_ = "stone wall";
            tile->ascii_ = '#';
            tile->colour_ = Colour::WHITE;
            tile->set_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::BlocksMovement);
            break;

        case TileID::STAIRS_DOWN:   // Stairs leading down to the dungeon level below.
            tile->name_ = "stairs down";
            tile->ascii_ = '>';
            tile->colour_ = Colour::WHITE_BOLD;
            tile->set_tag(TileTag::Immutable);
            tile->set_tag(TileTag::StairsDown);
            break;

        case TileID::STAIRS_UP: // Stairs leading up to the dungeon level (or surface world) above.
            tile->name_ = "stairs up";
            tile->ascii_ = '<';
            tile->colour_ = Colour::WHITE_BOLD;
            tile->set_tag(TileTag::Immutable);
            tile->set_tag(TileTag::StairsDown);
            break;

        case TileID::WALL_BEDROCK:  // Indestructible walls surrounding the map.
            tile->name_ = "bedrock wall";
            tile->ascii_ = '#';
            tile->colour_ = Colour::BLACK_BOLD;
            tile->set_tag(TileTag::Immutable);
            tile->set_tag(TileTag::BlocksMovement);
            tile->set_tag(TileTag::BlocksLight);
            break;

        case TileID::LG_FLOOR:  // Dungeon generation: will become walkable floor.
            tile->name_ = "unfinished floor";
            tile->ascii_ = '.';
            tile->colour_ = Colour::BLACK_BOLD;
            break;

        case TileID::LG_WALL:   // Dungeon generation: will become a solid wall.
            tile->name_ = "unfinished wall";
            tile->ascii_ = '#';
            tile->colour_ = Colour::BLACK_BOLD;
            tile->set_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::BlocksMovement);
            break;

        case TileID::LG_DOOR_CANDIDATE: // Dungeon generation: may become a door.
            tile->name_ = "door candidate";
            tile->ascii_ = '+';
            tile->colour_ = Colour::YELLOW;
            tile->set_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::BlocksMovement);
            break;

        case TileID::LG_FLOOR_CANDIDATE:    // Dungeon generation: may become floor.
            tile->name_ = "floor candidate";
            tile->ascii_ = '.';
            tile->colour_ = Colour::BLACK_BOLD;
            break;

        case TileID::DRUJ_TOMB: // Druj tombs, which spawn undead.
            tile->name_ = "druj tomb";
            tile->ascii_ = '&';
            tile->colour_ = Colour::YELLOW;
            tile->set_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::BlocksMovement);
            break;

        case TileID::DOOR_WOOD: // Wooden door
            tile->name_ = "wooden door";
            tile->ascii_ = '+';
            tile->colour_ = Colour::YELLOW;
            tile->set_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::Openable);
            break;
    }
}

}   // namespace invictus
