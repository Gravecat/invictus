// area/tile.cpp -- The Tile class, defining floors, walls, and other semi-permanent obstacles in the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "terminal/terminal-shared-defs.hpp"
#include "area/tile.hpp"


namespace invictus
{

// Constructor.
Tile::Tile() : ascii_('.'), colour_(Colour::WHITE), name_("tile") { }

// Get the ASCII character for this Tile.
char Tile::ascii() const { return ascii_; }

// Clears a TileTag from this Tile.
void Tile::clear_tag(TileTag the_tag)
{
    if (!(tags_.count(the_tag) > 0)) return;
    tags_.erase(the_tag);
}

// Gets the colour of this Tile.
Colour Tile::colour() const { return colour_; }

// Gets the name of this Tile.
std::string Tile::name() const { return name_; }

// Sets this Tile's colour.
void Tile::set_colour(Colour new_colour) { colour_ = new_colour; }

// Sets a TileTag on this Tile.
void Tile::set_tag(TileTag the_tag)
{
    if (tags_.count(the_tag) > 0) return;
    tags_.insert(the_tag);
}

// Checks if a TileTag is set on this Tile.
bool Tile::tag(TileTag the_tag) const { return (tags_.count(the_tag) > 0); }

}   // namespace invictus
