// entity/entity.hpp -- The Entity class defines tangible ~things~ in the game world, including the player, items, and monsters.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp
class Item;     // defined in entity/item.hpp
class Window;   // defined in terminal/window.hpp


// Various properties that can be on Entities and their derived classes.
enum class EntityProp : uint16_t
{
    ARMOUR,         // The defensive value of a piece of worn armour.
    DAMAGE_DICE_A,  // The amount of damage dice to roll.
    DAMAGE_DICE_B,  // The number of faces on each damage die.
    HP_OVERRIDE,    // Overrides the automatically-calculated HP value with something else.
    LIGHT_POWER,    // The power of a light source.
    MAX_FINESSE,    // The maximum amount of finesse bonus that can be used when this Item is equipped.
    SPEED,          // The movement speed of a Mobile, or the speed at which an Item can be used.
};

// Binary tags that can be set on all kinds of Entities.
enum class EntityTag : uint16_t
{
    None = 0,   // This is not a tag.

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

    // 20000 - 20999 -- General-purpose tags regarding this Mobile's state of being.
    Unliving = 20000,       // This Mobile is either undead or an unliving construct.
	ImmunityBleed,          // This Mobile cannot bleed.
	ImmunityPoison,         // This Mobile cannot be poisoned.


    // 21000 - 21999 -- Combat-related tags.
	CannotBlock = 21000,    // This Mobile cannot use a shield to block in combat.
	CannotDodge,            // This Mobile is unable to dodge attacks.
	CannotParry,            // This Mobile is unable to parry attacks.
    Passive,                // This Mobile will not attack the player unless the player strikes first.
    Blind,                  // If this tag is NOT set, the Mobile will hunt the player by sight.

    // 22000 - 22999 -- Temporary tags, which are set during gameplay.
    NoDeathMessage = 23000, // When this Mobile dies, it will not display a death message.


    /*****************************************
     * 30000 - 30999  --  ITEM-SPECIFIC TAGS *
     *****************************************/

    // 30000 - 32999 -- Generic item tags not specific to any particular type of item.

    // 33000 - 33499 -- Tags specific to weapons.
    TwoHanded = 33000,  // This Item is a weapon which takes both hands to wield.
    HandAndAHalf,       // This Item is a weapon which can be equipped in either one or both hands.
    WeaponRanged,       // This Item is a ranged weapon.
    WeaponFinesse,      // This weapon can get hit/damage bonuses from either Might or Finesse.
    WeaponLight,        // This weapon does not have a penalty when dual-wielding.

    // 33500 - 33999 -- Tags specific to armour.
    ArmourLight = 33500,    // This armour is lightweight, and allows for adding the user's Finesse bonus.
    ArmourMedium,           // This armour is medium, and partially limits the user's Finesse bonus.
    ArmourHeavy,            // This armour is heavy, and allows no Finesse bonus at all.
};

// Types of Entity and derived classes.
enum class EntityType : uint8_t { ENTITY, MOBILE, PLAYER, ITEM };


constexpr int   NAME_FLAG_THE =                 1;  // Prepends 'the' to the name, if appropriate.
constexpr int   NAME_FLAG_CAPITALIZE_FIRST =    2;  // Capitalizes the first letter of the name.
constexpr int   NAME_FLAG_POSSESSIVE =          4;  // Possessive version of the name (e.g. snake -> snake's).
constexpr int   NAME_FLAG_PLURAL =              8;  // Plural version of the name (e.g. rat -> rats).
constexpr int   NAME_FLAG_STACK =               16; // Show item stacks in the name.
constexpr int   NAME_FLAG_A =                   32; // Prepends 'a' to the name, if appropriate.


class Entity
{
public:
                        Entity();       // Constructor, creates a new Entity with default values.
    virtual             ~Entity() { }   // Virtual destructor.
    char                ascii() const;  // Gets the ASCII character representing this Entity.
    virtual bool        blocks_tile(int x_tile, int y_tile) const = 0;  // Checks if this Entity blocks a specified tile.
    void                clear_tag(EntityTag the_tag);   // Clears an EntityTag from this Entity.
    void                clear_tags(std::initializer_list<EntityTag> tag_list);  // Clears multiple EntityTags from this Entity.
    Colour              colour() const; // Gets the colour of this Entity.
    float               distance_from(int tile_x, int tile_y) const;            // Gets this Entity's distance from a specified tile.
    float               distance_from(std::shared_ptr<Entity> entity) const;    // As above, but measuring distance to an Entity.
    std::vector<std::shared_ptr<Item>>* inv();          // Returns the inventory pointer.
    void                inventory_add(std::shared_ptr<Entity> entity);  // Adds an Entity to this Entity's inventory.
    void                inventory_add(std::shared_ptr<Item> item);      // As above, but for an Entity in Item form.
    bool                is_at(int ax, int ay) const;    // Checks if this Entity claims to be occupying a specified tile.
    bool                is_at(std::shared_ptr<Entity> entity) const;    // As above, but checks against another Entity's position.
    bool                is_in_fov() const;              // Can this Entity be seen by the player?
    int32_t             light_power() const;            // Returns the power of this Entity's light source, if any.
    std::string name(int flags = 0) const;              // Retrieves this Entity's name.
    void                set_ascii(char new_ascii);      // Sets this Entity's ASCII character.
    void                set_colour(Colour new_colour);  // Sets this Entity's colour.
    void                set_light_power(int new_power); // Sets the power of this Entity's light source.
    void                set_name(const std::string &new_name);  // Sets this Entity's name.
    void                set_pos(int x, int y);          // Sets this Entity's coordinates.
    void                set_tag(EntityTag the_tag);     // Sets an EntityTag on this Entity.
    void                set_tags(std::initializer_list<EntityTag> tag_list);    // Sets multiple EntityTags on this Entity.
    bool                tag(EntityTag the_tag) const;   // Checks if an EntityTag is on this Entity.
    bool                tags(std::initializer_list<EntityTag> tag_list) const;  // Checks if multiple EntityTags are all set on this Entity.
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
    void        set_props(std::initializer_list<std::pair<EntityProp, int32_t>> prop_pairs);    // Sets multiple entity properties (int) at once.
    void        set_props_f(std::initializer_list<std::pair<EntityProp, float>> prop_pairs);    // Sets multiple entity properties (float) at once.

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
friend class CodexMobile;
friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_ENTITY_HPP_
