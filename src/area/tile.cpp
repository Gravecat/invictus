// area/tile.cpp -- The Tile class, defining floors, walls, and other semi-permanent obstacles in the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/tile.hpp"
#include "codex/codex-tile.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"


namespace invictus
{

// Constructor.
Tile::Tile() : ascii_(ASCII_NOTHING), ascii_scars_(ASCII_NOTHING), colour_(Colour::WHITE), colour_scars_(Colour::WHITE), id_(TileID::VOID_TILE), name_("tile")
{ }

// Get the ASCII character for this Tile.
char Tile::ascii(bool ignore_scars) const
{
    if (ignore_scars) return ascii_;
    else if (tag(TileTag::Bloodied)) return ascii_scars_;
    else return ascii_;
}

// Clears a TileTag from this Tile.
void Tile::clear_tag(TileTag the_tag, bool changed)
{
    if (!(tags_.count(the_tag) > 0)) return;
    tags_.erase(the_tag);
    if (changed && the_tag != TileTag::Changed) set_tag(TileTag::Changed);
}

// Clears multiple TileTags from this Tile.
void Tile::clear_tags(std::initializer_list<TileTag> tag_list, bool changed) { for (auto &the_tag : tag_list) clear_tag(the_tag, changed); }

// Gets the colour of this Tile.
Colour Tile::colour(bool ignore_scars) const
{
    if (ignore_scars) return colour_;
    else if (tag(TileTag::Bloodied)) return colour_scars_;
    else return colour_;
}

// Retrieves the ID of this Tile.
TileID Tile::id() const { return id_; }

// Checks if this Tile is identical to another.
bool Tile::is_identical_to(Tile* tile)
{
    if (id_ != tile->id_ || ascii_ != tile->ascii_ || colour_ != tile->colour_ || name_.compare(tile->name_)) return false;
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
void Tile::set_ascii(char new_ascii)
{
    ascii_ = new_ascii;
    set_tag(TileTag::Changed);
}

// Sets this Tile's colour.
void Tile::set_colour(Colour new_colour)
{
    colour_ = new_colour;
    set_tag(TileTag::Changed);
}

// Sets this Tile's name.
void Tile::set_name(const std::string &new_name)
{
    name_ = new_name;
    set_tag(TileTag::Changed);
}

// Sets the ASCII character and colour of this Tile, from blood/burns/etc.
void Tile::set_scars(char ch, Colour col)
{
    ascii_scars_ = ch;
    colour_scars_ = col;
}

// Sets a TileTag on this Tile.
void Tile::set_tag(TileTag the_tag, bool changed)
{
    if (tags_.count(the_tag) > 0) return;
    tags_.insert(the_tag);
    if (changed && the_tag != TileTag::Changed) set_tag(TileTag::Changed);
}

// Sets multiple TileTags on this Tile.
void Tile::set_tags(std::initializer_list<TileTag> tag_list, bool changed) { for (auto &the_tag : tag_list) set_tag(the_tag, changed); }

// Checks if a TileTag is set on this Tile.
bool Tile::tag(TileTag the_tag) const { return (tags_.count(the_tag) > 0); }

// Checks if multiple TileTags are all set on this Tile.
bool Tile::tags(std::initializer_list<TileTag> tag_list) const
{
    for (auto &the_tag : tag_list)
        if (!tag(the_tag)) return false;
    return true;
}

}   // namespace invictus
