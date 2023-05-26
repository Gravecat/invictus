// entity/entity.hpp -- The Entity class defines tangible ~things~ in the game world, including the player, items, and monsters.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <string>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp
class Window;   // defined in terminal/window.hpp


// Various properties that can be on Entities and their derived classes.
enum class EntityProp : uint8_t
{
    LIGHT_POWER, BLEED, CRIT, POISON, POWER, RANGE, SPEED
};

// Types of Entity and derived classes.
enum class EntityType : uint8_t { ENTITY, MOBILE, PLAYER, ITEM };


class Entity
{
public:
                        Entity();       // Constructor, creates a new Entity with default values.
    virtual             ~Entity() { }   // Virtual destructor.
    char                ascii() const;  // Gets the ASCII character representing this Entity.
    Colour              colour() const; // Gets the colour of this Entity.
    virtual float       distance_from(int tile_x, int tile_y) const;            // Gets this Entity's distance from a specified tile.
    virtual float       distance_from(std::shared_ptr<Entity> entity) const;    // As above, but measuring distance to an Entity.
    virtual bool        is_at(int ax, int ay) const;    // Checks if this Entity claims to be occupying a specified tile.
    virtual bool        is_in_fov() const;              // Can this Entity be seen by the player?
    int                 light_power() const;    // Returns the power of this Entity's light source, if any.
    std::string         name() const;   // Retrieves this Entity's name.
    void                set_ascii(char new_ascii);      // Sets this Entity's ASCII character.
    void                set_colour(Colour new_colour);  // Sets this Entity's colour.
    void                set_light_power(int new_power); // Sets the power of this Entity's light source.
    void                set_name(const std::string &new_name);  // Sets this Entity's name.
    void                set_pos(int x, int y);  // Sets this Entity's coordinates.
    virtual void        tick(std::shared_ptr<Entity> self) = 0;     // Updates the state of this Entity or takes an AI action.
    virtual void        tick10(std::shared_ptr<Entity> self) = 0;   // As above, but for slower events such as buffs/debuffs ticking.
    virtual EntityType  type() const = 0;   // Pure virtual, to ensure this is an abstract class. Derived classes will report their class type here.
    uint16_t            x() const;      // Read-only access to the Entity's X coordinate.
    uint16_t            y() const;      // Read-only access to the Entity's Y coordinate.

protected:
    int32_t     get_prop(EntityProp prop) const;            // Retrieves an entity property (int), or returns 0 if it is not present.
    float       get_prop_f(EntityProp prop) const;          // Retrieves an entity property (float), or returns 0 if it is not present.
    void        set_prop(EntityProp prop, int32_t value);   // Sets an entity property (int).
    void        set_prop_f(EntityProp prop, float value);   // Sets an entity property (float).

private:
    char        ascii_;     // The ASCII character representing this Entity.
    Colour      colour_;    // The colour of this Entity.
    std::map<EntityProp, float>     entity_properties_f_;   // Various properties that can be on this Entity (floats).
    std::map<EntityProp, int32_t>   entity_properties_i_;   // Various properties that can be on this Entity (ints).
    std::string name_;      // The name of this Entity.
    uint16_t    x_, y_;     // Position on the map.

friend class CodexItem;
};

}       // namespace invictus
#endif  // ENTITY_ENTITY_HPP_
