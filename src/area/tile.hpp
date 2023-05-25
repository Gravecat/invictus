// area/tile.hpp -- The Tile class, defining floors, walls, and other semi-permanent obstacles in the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef AREA_TILE_HPP_
#define AREA_TILE_HPP_

#include <cstdint>
#include <set>
#include <string>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp


// Tags on a Tile. They're like flags except we don't risk running out of bits on an integer.
enum class TileTag : uint16_t
{
    // Basic tags that are common to many Tile types.
    BlocksMovement = 1, // This Tile blocks anything from moving onto it.
    BlocksLight = 2,    // This Tile blocks the passage of light.
    Immutable = 3,      // This Tile is important and should not be changed.

    // Transient tags which are (usually) not defined by default, but are set by player behaviour.
    Explored = 100,     // This Tile has been seen and memorized by the player.
    Bloodied = 102,     // This Tile has been splashed with blood and gore.
    Open = 103,         // This Tile is currently open.

    // Special types of Tiles.
    StairsUp = 200,     // This Tile is an upward stairway.
    StairsDown = 201,   // This Tile is a downward stairway.
    Openable = 202,     // This Tile is a door that can be opened.
    Closeable = 203,    // This Tile is a door that can be closed.
};


class Tile
{
public:
                Tile();     // Constructor.
    void        clear_tag(TileTag the_tag); // Clears a TileTag from this Tile.
    char        ascii() const;  // Get the ASCII character for this Tile.
    Colour      colour() const; // Gets the colour of this Tile.
    std::string name() const;   // Gets the name of this Tile.
    void        set_ascii(char new_ascii);      // Sets this Tile's ASCII character.
    void        set_colour(Colour new_colour);  // Sets this Tile's colour.
    void        set_name(const std::string &new_name);  // Sets this Tile's name.
    void        set_tag(TileTag the_tag);   // Sets a TileTag on this Tile.
    bool        tag(TileTag the_tag) const; // Checks if a TileTag is set on this Tile.

private:
    char        ascii_;     // The ASCII character used to represent this Tile.
    Colour      colour_;    // The colour of this Tile.
    std::string name_;      // The name of this Tile.
    std::set<TileTag>   tags_;  // Any and all TileTags on this Tile.

friend class FactoryTile;
};

}       // namespace invictus
#endif  // AREA_TILE_HPP_
