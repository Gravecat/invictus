// entity/mobile.cpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/item.hpp"
#include "entity/mobile.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/timing.hpp"
#include "util/strx.hpp"


namespace invictus
{

// A blank item, shared amongst all Mobiles to use in their empty equipment slots.
std::shared_ptr<Item> Mobile::blank_item_ = std::make_shared<Item>();


// Constructor.
Mobile::Mobile() : Entity(), banked_ticks_(0), hp_{100, 100}, last_dir_(0), mp_{100, 100}, sp_{100, 100}
{
    set_name("mobile");
    set_prop_f(EntityProp::SPEED, TIME_BASE_MOVEMENT);

    // Populates the equipment vector with blank items.
    for (unsigned int i = 0; i < static_cast<unsigned int>(EquipSlot::_END); i++)
        equipment_.push_back(blank_item_);
}

// Adds or removes banked ticks to this Mobile.
void Mobile::add_banked_ticks(float amount)
{
    if (amount < 0) core()->guru()->halt("Attempt to add negative banked ticks to " + name(), static_cast<int>(amount));
    else banked_ticks_ += amount;
}

// Retrieves the amount of ticks banked by this Mobile.
float Mobile::banked_ticks() const { return banked_ticks_; }

// Erase all banked ticks on this Mobile.
void Mobile::clear_banked_ticks() { banked_ticks_ = 0; }

// Attempts to close a door.
void Mobile::close_door(int dx, int dy)
{
    auto area = core()->game()->area();
    bool success = true;
    const bool is_player = (type() == EntityType::PLAYER);
    if (!is_player && banked_ticks() < TIME_CLOSE_DOOR) return;
    for (auto mobile : *area->entities())
    {
        if (mobile->is_at(dx, dy))
        {
            success = false;
            break;
        }
    }

    auto the_tile = area->tile(dx, dy);
    std::string door_name = the_tile->name();
    StrX::find_and_replace(door_name, " (open)", "");
    if (!success)
    {
        if (is_player) core()->message("{y}You can't close it, something seems to be blocking the " + door_name + ".");
        return;
    }

    if (is_player) core()->message("You close the " + door_name + ".");
    else
    {
        if (area->is_in_fov(dx, dy))
        {
            if (is_in_fov()) core()->message("{u}You see " + name(NAME_FLAG_A) + " {u}close a " + door_name + "{u}.");
            else core()->message("{u}You see a " + door_name + " close.");
        }
    }
    
    the_tile->set_ascii(ASCII_DOOR_CLOSED);
    the_tile->set_tags({TileTag::Openable, TileTag::BlocksLight});
    the_tile->clear_tags({TileTag::Closeable, TileTag::Open});
    area->need_fov_recalc();
    timed_action(TIME_CLOSE_DOOR);
}

// Drops a carried item.
void Mobile::drop_item(uint32_t id)
{
    if (id >= inv()->size()) core()->guru()->halt("Invalid item ID for drop", id, inv()->size());
    if (type() != EntityType::PLAYER && banked_ticks() < TIME_DROP_ITEM) return;
    std::shared_ptr<Entity> item = inv()->at(id);
    core()->game()->area()->entities()->push_back(item);
    inv()->erase(inv()->begin() + id);
    item->set_pos(x(), y());

    if (type() == EntityType::PLAYER) core()->message("You drop {c}" + item->name(NAME_FLAG_THE) + " {w}on the ground.");
    else if (is_in_fov()) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " {u}drops " + item->name(NAME_FLAG_A) +
        " {u}on the ground.");
    timed_action(TIME_DROP_ITEM);
}

// Retrieves a pointer to the equipment vector.
std::vector<std::shared_ptr<Item>>* Mobile::equ() { return &equipment_; }

// Equips a specified Item.
void Mobile::equip_item(uint32_t id)
{
    if (id >= inv()->size()) core()->guru()->halt("Invalid inventory slot", id, inv()->size());
    EquipSlot target_slot = EquipSlot::_END;
    float time_taken = 0;
    auto item = inv()->at(id);
    std::string target_slot_name;

    if (item->item_type() == ItemType::WEAPON)
    {
        auto main_item = equipment(EquipSlot::HAND_MAIN);
        auto off_item = equipment(EquipSlot::HAND_OFF);

        bool unequips_main = false, unequips_off = false;
        bool main_used = (main_item->item_type() != ItemType::NONE);
        bool off_used = ((off_item->item_type() != ItemType::NONE) || main_item->tag(EntityTag::TwoHanded));
        time_taken = TIME_EQUIP_WEAPON;

        if (item->tag(EntityTag::TwoHanded))
        {
            if (main_used) unequips_main = true;
            if (off_used) unequips_off = true;
            target_slot = EquipSlot::HAND_MAIN;
        }
        else
        {
            if (main_used)
            {
                if (off_used)
                {
                    unequips_main = true;
                    target_slot = EquipSlot::HAND_MAIN;
                }
                else target_slot = EquipSlot::HAND_OFF;
            }
            else target_slot = EquipSlot::HAND_MAIN;
        }
        
        if (unequips_main) unequip_item(EquipSlot::HAND_MAIN);
        if (unequips_off) unequip_item(EquipSlot::HAND_OFF);
    }
    else if (item->item_type() == ItemType::ARMOUR)
    {
        switch(item->item_subtype())
        {
            case ItemSub::BODY: target_slot = EquipSlot::BODY; time_taken = TIME_EQUIP_ARMOUR_BODY; break;
            case ItemSub::HEAD: target_slot = EquipSlot::HEAD; time_taken = TIME_EQUIP_ARMOUR_HEAD; break;
            case ItemSub::HANDS: target_slot = EquipSlot::HANDS; time_taken = TIME_EQUIP_ARMOUR_HANDS; break;
            case ItemSub::FEET: target_slot = EquipSlot::FEET; time_taken = TIME_EQUIP_ARMOUR_FEET; break;
            default: core()->guru()->halt("Unable to determine armour slot: " + item->name()); return;
        }
    }
    else if (item->item_type() == ItemType::SHIELD)
    {
        auto main_item = equipment(EquipSlot::HAND_MAIN);
        auto off_item = equipment(EquipSlot::HAND_OFF);
        if (main_item->tag(EntityTag::TwoHanded)) unequip_item(EquipSlot::HAND_MAIN);
        if (off_item->item_type() != ItemType::NONE) unequip_item(EquipSlot::HAND_OFF);
        target_slot = EquipSlot::HAND_OFF;
        time_taken = TIME_EQUIP_SHIELD;
    }
    else core()->guru()->halt("Unable to equip: " + item->name());

    if (target_slot == EquipSlot::_END) core()->guru()->halt("Unable to determine equipment slot: " + item->name());

    switch(target_slot)
    {
        case EquipSlot::HAND_MAIN:
            if (item->tag(EntityTag::TwoHanded) || (item->tag(EntityTag::HandAndAHalf) && equipment(EquipSlot::HAND_OFF)->item_type() == ItemType::NONE))
                target_slot_name = "in both hands";
            else target_slot_name = "in your main hand";
            break;
        case EquipSlot::HAND_OFF: target_slot_name = "in your off hand"; break;
        case EquipSlot::BODY: target_slot_name = "on your body"; break;
        case EquipSlot::HEAD: target_slot_name = "on your head"; break;
        case EquipSlot::HANDS: target_slot_name = "on your hands"; break;
        case EquipSlot::FEET: target_slot_name = "on your feet"; break;
        case EquipSlot::_END: break;    // To keep the compiler happy.
    }

    equ()->at(static_cast<unsigned int>(target_slot)) = item;
    inv()->erase(inv()->begin() + id);
    if (type() == EntityType::PLAYER)
    {
        if (item->item_type() == ItemType::WEAPON) core()->message("You wield {c}" + item->name(NAME_FLAG_THE) + " {w}" + target_slot_name + ".");
        else if (item->item_type() == ItemType::SHIELD) core()->message("You hold {c}" + item->name(NAME_FLAG_THE) + " {w}" + target_slot_name + ".");
        else core()->message("You wear {c}" + item->name(NAME_FLAG_THE) + " " + target_slot_name + ".");
    }
    else if (is_in_fov())
    {
        if (item->item_type() == ItemType::WEAPON) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " wields " +
            item->name(NAME_FLAG_A) + "{u}.");
        else if (item->item_type() == ItemType::SHIELD) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " holds " +
            item->name(NAME_FLAG_A) + "{u}.");
        else core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " wears " + item->name(NAME_FLAG_A) + "{u}.");
    }

    timed_action(time_taken);
}

// Retrieves equipment from a given slot.
std::shared_ptr<Item> Mobile::equipment(EquipSlot slot)
{
    const uint32_t slot_id = static_cast<uint32_t>(slot);
    if (slot >= EquipSlot::_END) core()->guru()->halt("Invalid equipment slot", slot_id);
    return equ()->at(slot_id);
}

// Retrieves the current or maximum hit points of this Mobile.
uint16_t Mobile::hp(bool max) const { return hp_[max ? 1 : 0]; }

// Checks if this Mobile is dead.
bool Mobile::is_dead() const { return !hp(); }

// Moves in a given direction, or attacks something in the destination tile
bool Mobile::move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy)
{
    auto game = core()->game();

    if (!dx && !dy)
    {
        core()->guru()->nonfatal("move_or_attack called with no direction!", GURU_WARN);
        return false;
    }
    const bool is_player = self->type() == EntityType::PLAYER;
    int xdx = x() + dx, ydy = y() + dy;
    auto area = game->area();
    if (area->can_walk(xdx, ydy))
    {
        auto the_tile = area->tile(xdx, ydy);
        const bool openable = (the_tile->tag(TileTag::Openable));
        const float movement_cost = (openable ? TIME_OPEN_DOOR : self->movement_speed());
        if (!is_player && banked_ticks() < movement_cost) return false;

        if (openable)
        {
            if (is_player) core()->message("You open the " + the_tile->name() + ".");
            else if (area->is_in_fov(xdx, ydy))
            {
                if (is_in_fov()) core()->message("{u}You see " + name(NAME_FLAG_THE) + " {u}open a " + the_tile->name() + "{u}.");
                else core()->message("{u}You see a " + the_tile->name() + " {u}open.");
            }
            auto tile = area->tile(xdx, ydy);
            tile->set_ascii(ASCII_DOOR_OPEN);
            tile->clear_tags({TileTag::Openable, TileTag::BlocksLight});
            tile->set_tags({TileTag::Closeable, TileTag::Open});
            area->need_fov_recalc();
            timed_action(TIME_OPEN_DOOR);
            return true;
        }

        set_pos(xdx, ydy);
        last_dir_ = ((dx + 2) << 4) + (dy + 2);
        area->need_fov_recalc();

        if (is_player)
        {
            std::vector<std::string> floor_items;
            for (auto entity : *area->entities())
            {
                if (entity == self || !entity->is_at(self)) continue;
                floor_items.push_back(entity->name());
            }
            if (floor_items.size()) core()->message("You see {c}" + StrX::comma_list(floor_items, true) + " {w}here.", 0);
            Tile* self_tile = area->tile(xdx, ydy);
            if (self_tile->tag(TileTag::StairsDown)) core()->message("You see a staircase leading downward.");
            else if (self_tile->tag(TileTag::StairsUp)) core()->message("You see a staircase leading upward.");
            else if (self_tile->tag(TileTag::Open))
            {
                std::string door_name = self_tile->name();
                StrX::find_and_replace(door_name, " (open)", "");
                core()->message("You pass through an open " + door_name + ".");
            }
        }

        timed_action(movement_cost);
        return true;
    }

    // We're not taking an action right now.
    if (!is_player) clear_banked_ticks();
    return false;
}

// Returns the amount of ticks needed for this Mobile to move one tile.
float Mobile::movement_speed() const { return get_prop_f(EntityProp::SPEED); }

// Retrieves the current or maximum mana points of this Mobile.
uint16_t Mobile::mp(bool max) const { return mp_[max ? 1 : 0]; }

// Retrieves the current or maximum stamina points of this Mobile.
uint16_t Mobile::sp(bool max) const { return sp_[max ? 1 : 0]; }

// Picks up a specified item.
void Mobile::take_item(uint32_t id)
{
    auto area = core()->game()->area();
    auto entities = area->entities();
    if (entities->size() <= id) core()->guru()->halt("Attempt to pick up invalid item ID.", id);
    std::shared_ptr<Entity> entity = entities->at(id);
    if (entity->type() != EntityType::ITEM) core()->guru()->halt("Attempt to pick up non-item entity.", id);
    if (type() != EntityType::PLAYER && banked_ticks() < TIME_TAKE_ITEM) return;

    inventory_add(entity);
    entities->erase(entities->begin() + id);
    if (type() == EntityType::PLAYER) core()->message("You pick up {c}" + entity->name(NAME_FLAG_A) + "{w}.");
    else if (is_in_fov()) core()->message("{u}" + name() + " {u}picks up " + entity->name(NAME_FLAG_A) + "{u}.");
    timed_action(TIME_TAKE_ITEM);
}

// Processes AI for this Mobile each turn.
void Mobile::tick(std::shared_ptr<Entity> self)
{
    Entity::tick(self);
    if (is_dead()) return;

    if (type() == EntityType::PLAYER) return;

    add_banked_ticks(TICK_SPEED);
    auto self_mob = std::dynamic_pointer_cast<Mobile>(self);

    // There's nothing more to do.
    clear_banked_ticks();
}

// Process slower state-change events that happen less often, such as buffs/debuffs ticking.
void Mobile::tick10(std::shared_ptr<Entity> self)
{
    Entity::tick10(self);
    if (is_dead()) return;
}

// This Mobile has made an action which takes time. Handles both Mobile and Player differences internally.
void Mobile::timed_action(float time_taken)
{
    if (type() == EntityType::PLAYER) core()->game()->pass_time(time_taken);
    else banked_ticks_ -= time_taken;
}

// Unequips a specified Item.
void Mobile::unequip_item(EquipSlot slot)
{
    const unsigned int slot_id = static_cast<unsigned int>(slot);
    if (slot >= EquipSlot::_END) core()->guru()->halt("Invalid equipment slot!", slot_id);
    auto item = equ()->at(slot_id);
    if (item->item_type() == ItemType::NONE) core()->guru()->halt("Attempt to unequip null item!", slot_id);

    float time_taken = 0;
    switch(slot)
    {
        case EquipSlot::HAND_MAIN: case EquipSlot::HAND_OFF:
            if (item->item_type() == ItemType::WEAPON) time_taken = TIME_UNEQUIP_WEAPON;
            else if (item->item_type() == ItemType::SHIELD) time_taken = TIME_UNEQUIP_SHIELD;
            else core()->guru()->halt("Unable to determine item type", static_cast<uint32_t>(item->item_type()));
            break;
        case EquipSlot::BODY: time_taken = TIME_UNEQUIP_ARMOUR_BODY; break;
        case EquipSlot::HANDS: time_taken = TIME_UNEQUIP_ARMOUR_HANDS; break;
        case EquipSlot::HEAD: time_taken = TIME_UNEQUIP_ARMOUR_HEAD; break;
        case EquipSlot::FEET: time_taken = TIME_UNEQUIP_ARMOUR_FEET; break;
        case EquipSlot::_END: break;    // Will never happen, but this keeps the compiler happy.
    }
    inventory_add(item);
    equipment_.at(slot_id) = blank_item_;

    if (type() == EntityType::PLAYER) core()->message("You remove {c}" + item->name(NAME_FLAG_THE) + "{w}.");
    else if (is_in_fov()) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " {u}removes " + item->name(NAME_FLAG_A) + "{u}.");

    timed_action(time_taken);
}

}   // namespace invictus
