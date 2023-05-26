// entity/entity.hpp -- The Entity class defines tangible ~things~ in the game world, including the player, items, and monsters.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp
class Item;     // defined in entity/item.hpp
class Window;   // defined in terminal/window.hpp


// Various properties that can be on Entities and their derived classes.
enum class EntityProp : uint16_t
{
    LIGHT_POWER, BLEED, CRIT, POISON, POWER, RANGE, SPEED
};

// Binary tags that can be set on all kinds of Entities.
enum class EntityTag : uint16_t
{
    /**********************************************
     * 1 - 9999  --  TAGS GENERIC TO ALL ENTITIES *
     **********************************************/

    ProperNoun = 1, // This Entity's name is a proper noun (e.g. Smaug).
    PluralName = 2, // This Entity's name is a plural (e.g. "pack of rats").
    NoA = 3,        // This Entity's name should not be prefaced with 'a' (e.g. 'plate mail armour' instead of 'a plate mail armour').


    /*******************************************
     * 10000 - 19999  --  PLAYER-SPECIFIC TAGS *
     *******************************************/


    /*******************************************
     * 20000 - 29999  --  MOBILE-SPECIFIC TAGS *
     *******************************************/


    /*****************************************
     * 30000 - 30999  --  ITEM-SPECIFIC TAGS *
     *****************************************/
};

// Types of Entity and derived classes.
enum class EntityType : uint8_t { ENTITY, MOBILE, PLAYER, ITEM };


class Entity
{
public:
    static constexpr int    NAME_FLAG_THE =                 1;  // Prepends 'the' to the name, if appropriate.
    static constexpr int    NAME_FLAG_CAPITALIZE_FIRST =    2;  // Capitalizes the first letter of the name.
    static constexpr int    NAME_FLAG_POSSESSIVE =          4;  // Possessive version of the name (e.g. snake -> snake's).
    static constexpr int    NAME_FLAG_PLURAL =              8;  // Plural version of the name (e.g. rat -> rats).
    static constexpr int    NAME_FLAG_STACK =               16; // Show item stacks in the name.
    static constexpr int    NAME_FLAG_A =                   32; // Prepends 'a' to the name, if appropriate.

                        Entity();       // Constructor, creates a new Entity with default values.
    virtual             ~Entity() { }   // Virtual destructor.
    char                ascii() const;  // Gets the ASCII character representing this Entity.
    void                clear_tag(EntityTag the_tag);   // Clears an EntityTag from this Entity.
    Colour              colour() const; // Gets the colour of this Entity.
    virtual float       distance_from(int tile_x, int tile_y) const;            // Gets this Entity's distance from a specified tile.
    virtual float       distance_from(std::shared_ptr<Entity> entity) const;    // As above, but measuring distance to an Entity.
    std::vector<std::shared_ptr<Item>>* inv();          // Returns the inventory pointer.
    void                inventory_add(std::shared_ptr<Entity> entity);  // Adds an Entity to this Entity's inventory.
    virtual bool        is_at(int ax, int ay) const;    // Checks if this Entity claims to be occupying a specified tile.
    virtual bool        is_at(std::shared_ptr<Entity> entity) const;    // As above, but checks against another Entity's position.
    virtual bool        is_in_fov() const;              // Can this Entity be seen by the player?
    int                 light_power() const;            // Returns the power of this Entity's light source, if any.
    std::string         name(int flags = 0) const;      // Retrieves this Entity's name.
    void                set_ascii(char new_ascii);      // Sets this Entity's ASCII character.
    void                set_colour(Colour new_colour);  // Sets this Entity's colour.
    void                set_light_power(int new_power); // Sets the power of this Entity's light source.
    void                set_name(const std::string &new_name);  // Sets this Entity's name.
    void                set_pos(int x, int y);          // Sets this Entity's coordinates.
    void                set_tag(EntityTag the_tag);     // Sets an EntityTag on this Entity.
    bool                tag(EntityTag the_tag) const;   // Checks if an EntityTag is on this Entity.
    virtual void        tick(std::shared_ptr<Entity> self);     // Updates the state of this Entity or takes an AI action.
    virtual void        tick10(std::shared_ptr<Entity> self);   // As above, but for slower events such as buffs/debuffs ticking.
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
    std::vector<std::shared_ptr<Item>>    inventory_;       // The things carried by this Entity.
    std::string name_;          // The name of this Entity.
    std::set<EntityTag> tags_;  // Any and all EntityTags on this Entity.
    uint16_t    x_, y_;         // Position on the map.

friend class CodexItem;
};

}       // namespace invictus
#endif  // ENTITY_ENTITY_HPP_
