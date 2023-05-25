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

// Checks if this Tile is identical to another.
bool Tile::is_identical_to(Tile* tile)
{
    if (ascii_ != tile->ascii_ || colour_ != tile->colour_ || name_.compare(tile->name_)) return false;
    for (auto the_tag : tags_)
        if (!tile->tag(the_tag)) return false;
    for (auto the_tag : tile->tags_)
        if (!tag(the_tag)) return false;
    return true;
}

// Gets the name of this Tile.
std::string Tile::name() const
{
    if (tag(TileTag::Open)) return name_ + " (open)";
    return name_;
}

// Sets this Tile's ASCII character.
void Tile::set_ascii(char new_ascii) { ascii_ = new_ascii; }

// Sets this Tile's colour.
void Tile::set_colour(Colour new_colour) { colour_ = new_colour; }

// Sets this Tile's name.
void Tile::set_name(const std::string &new_name) { name_ = new_name; }

// Sets a TileTag on this Tile.
void Tile::set_tag(TileTag the_tag)
{
    if (tags_.count(the_tag) > 0) return;
    tags_.insert(the_tag);
}

// Checks if a TileTag is set on this Tile.
bool Tile::tag(TileTag the_tag) const { return (tags_.count(the_tag) > 0); }

}   // namespace invictus
