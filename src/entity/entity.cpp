// entity/entity.cpp -- The Entity class defines tangible ~things~ in the game world, including the player, items, and monsters.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "entity/entity.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"


namespace invictus
{

// Constructor, creates a new Entity with default values.
Entity::Entity() : ascii_('?'), colour_(Colour::WHITE), name_("entity"), x_(0), y_(0) { }

// Gets the ASCII character representing this Entity.
char Entity::ascii() const { return ascii_; }

// Gets the colour of this Entity.
Colour Entity::colour() const { return colour_; }

// Retrieves an entity property (int), or returns 0 if it is not present.
int32_t Entity::get_prop(EntityProp prop) const
{
    auto result = entity_properties_i_.find(prop);
    if (result == entity_properties_i_.end()) return 0;
    else return result->second;
}

// Retrieves an entity property (float), or returns 0 if it is not present.
float Entity::get_prop_f(EntityProp prop) const
{
    auto result = entity_properties_f_.find(prop);
    if (result == entity_properties_f_.end()) return 0;
    else return result->second;
}

// Checks if this Entity claims to be occupying a specified tile.
bool Entity::is_at(int ax, int ay) const
{
    if (ax < 0 || ay < 0) core()->guru()->nonfatal("Invalid call to Entity::is_at() on " + name_ + ": " + std::to_string(ax) + "," + std::to_string(ay),
        Guru::GURU_ERROR);
    return x_ == ax && y_ == ay;
}

// Returns the power of this Entity's light source, if any.
int Entity::light_power() const { return get_prop(EntityProp::LIGHT_POWER); }

// Retrieves this Entity's name.
std::string Entity::name() const { return name_; }

// Renders this Entity on the screen.
void Entity::render(std::shared_ptr<Window> window, int screen_x, int screen_y) const
{ core()->terminal()->put(ascii_, screen_x, screen_y, colour_, 0, window); }

// Sets this Entity's ASCII character.
void Entity::set_ascii(char new_ascii) { ascii_ = new_ascii; }

// Sets this Entity's colour.
void Entity::set_colour(Colour new_colour) { colour_ = new_colour; }

// Sets the power of this Entity's light source.
void Entity::set_light_power(int new_power)
{
    if (new_power < 0) core()->guru()->nonfatal("Invalid light power value on " + name_ + ": " + std::to_string(new_power), Guru::GURU_ERROR);
    set_prop(EntityProp::LIGHT_POWER, std::max<int>(new_power, 0));
}

// Sets this Entity's name.
void Entity::set_name(const std::string &new_name) { name_ = new_name; }

// Sets this Entity's coordinates.
void Entity::set_pos(int x, int y)
{
    if (x < 0 || y < 0) core()->guru()->nonfatal("Invalid call to Entity::set_pos on " + name_ + ": " + std::to_string(x) + "," + std::to_string(y),
        Guru::GURU_ERROR);
    x_ = std::max<int>(x, 0);
    y_ = std::max<int>(y, 0);
}

// Sets an entity property (int).
void Entity::set_prop(EntityProp prop, int32_t value)
{
    auto result = entity_properties_i_.find(prop);
    if (value)
    {
        if (result == entity_properties_i_.end()) entity_properties_i_.insert(std::pair<EntityProp, int32_t>(prop, value));
        else result->second = value;
    }
    else if (result == entity_properties_i_.end()) entity_properties_i_.erase(result->first);
}

// Sets an entity property (float).
void Entity::set_prop_f(EntityProp prop, float value)
{
    auto result = entity_properties_f_.find(prop);
    if (value)
    {
        if (result == entity_properties_f_.end()) entity_properties_f_.insert(std::pair<EntityProp, float>(prop, value));
        else result->second = value;
    }
    else if (result == entity_properties_f_.end()) entity_properties_f_.erase(result->first);
}

// Read-only access to the Entity's X coordinate.
uint16_t Entity::x() const { return x_; }

// Read-only access to the Entity's Y coordinate.
uint16_t Entity::y() const { return y_; }

}   // namespace invictus
