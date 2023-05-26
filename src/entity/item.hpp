// entity/item.hpp -- The Item class is a type of Entity we can pick up and use.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_ITEM_HPP_
#define ENTITY_ITEM_HPP_

#include <cstdint>

#include "entity/entity.hpp"


namespace invictus
{

// Types of Items in the game.
enum class ItemType : uint8_t { NONE, ARMOUR, POTION, SCROLL, SHIELD, WEAPON, AMMO };

// More specialized sub-types of Items.
enum class ItemSub : uint8_t { NONE,
    HEALING,    // Potions
    CONFUSION, FIREBALL, LIGHTNING_BOLT,    // Spells
    AXE, DAGGER, SWORD, UNARMED, BOW,       // Weapons
    BODY, HEAD, HANDS, FEET,                // Armour
    ARROW                                   // Ammo
};


class Item : public Entity
{
public:
                Item();                 // Constructor, sets default values.
    void        adjust_stack(int mod);  // Adds or removes from this Item stack.
    uint32_t    bleed() const;          // Retrieves the bleed chance % of this Item, if any.
    bool        blocks_tile(uint16_t tile_x, uint16_t tile_y) const;    // Checks if this Item blocks a specified tile.
    uint32_t    crit() const;           // Retrieves the critical chance % of this Item, if any.
    ItemSub     item_subtype() const;   // Returns the sub-type of this Item.
    ItemType    item_type() const;      // Returns the type of this Item.
    uint32_t    poison() const;         // Retrieves the poison chance % of this Item, if any.
    uint32_t    power() const;          // Retrieves the power level of this Item, if any.
    uint32_t    range() const;          // Retrieves the range of this Item, if any.
    void        set_stack(uint16_t size);   // Sets the stack size for this Item.
    float       speed() const;          // Retrieves the speed of this Item, if any.
    uint16_t    stack() const;          // Retrieves the size of this Item stack, if any.
    EntityType  type() const override { return EntityType::ITEM; }  // Identifier.

private:
    ItemSub     item_subtype_;  // The item's sub-type.
    ItemType    item_type_;     // The specific type of Item.
    uint16_t    stack_;         // The size of this Item's stack, if any.
};

}       // namespace invictus
#endif  // ENTITY_ITEM_HPP_
