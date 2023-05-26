// entity/item.cpp -- The Item class is a type of Entity we can pick up and use.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/guru.hpp"
#include "entity/item.hpp"


namespace invictus
{

// Constructor, sets default values.
Item::Item() : Entity(), item_subtype_(ItemSub::NONE), item_type_(ItemType::NONE), stack_(1) { set_name("item"); }

// Adds or removes from this Item stack.
void Item::adjust_stack(int mod)
{
    if ((-mod) >= stack_) core()->guru()->halt("Invalid stack operation on " + name(), stack_, mod);
    else stack_ += mod;
}

// Retrieves the bleed chance % of this Item, if any.
uint32_t Item::bleed() const { return get_prop(EntityProp::BLEED); }

// Checks if this Item blocks a specified tile.
bool Item::blocks_tile(uint16_t, uint16_t) const { return false; }

// Retrieves the critical chance % of this Item, if any.
uint32_t Item::crit() const { return get_prop(EntityProp::CRIT); }

// Returns the sub-type of this Item.
ItemSub Item::item_subtype() const { return item_subtype_; }

// Returns the type of this Item.
ItemType Item::item_type() const { return item_type_; }

// Retrieves the poison chance % of this Item, if any.
uint32_t Item::poison() const { return get_prop(EntityProp::POISON); }

// Retrieves the power level of this Item, if any.
uint32_t Item::power() const { return get_prop(EntityProp::POWER); }

// Retrieves the range of this Item, if any.
uint32_t Item::range() const { return get_prop(EntityProp::RANGE); }

// Sets the stack size for this Item.
void Item::set_stack(uint16_t size) { stack_ = size; }

// Retrieves the speed of this Item, if any.
float Item::speed() const { return get_prop_f(EntityProp::SPEED); }

// Retrieves the size of this Item stack, if any.
uint16_t Item::stack() const { return stack_; }

}   // namespace invictus
