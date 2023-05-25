// factory/factory-tile.hpp -- Factory class, generating preset types of Tiles that make up the game world's structure.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef FACTORY_FACTORY_TILE_HPP_
#define FACTORY_FACTORY_TILE_HPP_

#include <cstdint>


namespace invictus
{

class Tile; // defined in world/tile.hpp


enum class TileID : uint16_t { VOID_TILE = 0, FLOOR_STONE, WALL_STONE, STAIRS_DOWN, STAIRS_UP, WALL_BEDROCK, LG_FLOOR, LG_WALL, LG_DOOR_CANDIDATE,
    LG_FLOOR_CANDIDATE, DRUJ_TOMB, DOOR_WOOD };

class FactoryTile
{
public:
    static void generate(Tile* tile, TileID id);    // Generates a preset Tile.
};

}       // namespace invictus
#endif  // FACTORY_FACTORY_TILE_HPP_
