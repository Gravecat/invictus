// entity/mobile.cpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "area/area.hpp"
#include "area/gore.hpp"
#include "area/tile.hpp"
#include "codex/codex-tile.hpp"
#include "combat/combat.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/item.hpp"
#include "entity/monster.hpp"
#include "entity/player.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/combat.hpp"
#include "tune/gore.hpp"
#include "tune/resting.hpp"
#include "tune/timing.hpp"
#include "ui/ui.hpp"
#include "util/random.hpp"
#include "util/strx.hpp"


namespace invictus
{

// A blank item, shared amongst all Mobiles to use in their empty equipment slots.
std::shared_ptr<Item> Mobile::blank_item_ = std::make_shared<Item>();


// Constructor.
Mobile::Mobile() : Entity(), awake_(false), bloody_feet_(0), hp_{1, 1}, move_speed_(TIME_BASE_MOVEMENT), mp_{0, 0}, sp_{0, 0}
{
    // Populates the equipment vector with blank items.
    for (unsigned int i = 0; i < static_cast<unsigned int>(EquipSlot::_END); i++)
        equipment_.push_back(blank_item_);
}

// Increase (or decrease) the amount of blood on this Mobile's feet.
void Mobile::add_bloody_feet(float blood) { bloody_feet_ += blood; }

// Returns the total armour modifier from this Mobile and their equipped gear.
int Mobile::armour() { return equipment(EquipSlot::BODY)->armour(); }

// Returns the number of ticks needed for this Mobile to make an attack.
float Mobile::attack_speed() { return 1.0f; }

// Checks if this Mobile blocks a specified tile.
bool Mobile::blocks_tile(int x_tile, int y_tile) const
{ return (!is_dead() && x_tile == x() && y_tile == y()); }

// Checks how bloodied this Mobile's feet are.
float Mobile::bloody_feet() const { return bloody_feet_; }

// Attempts to close a door.
void Mobile::close_door(int dx, int dy)
{
    auto area = core()->game()->area();
    bool success = true;
    const bool is_player = (type() == EntityType::PLAYER);
    auto monster = (type() == EntityType::MONSTER ? dynamic_cast<Monster*>(this) : nullptr);

    if (!is_player && monster->banked_ticks() < TIME_CLOSE_DOOR) return;
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
            if (is_in_fov()) core()->message("{u}You see " + name(NAME_FLAG_A) + " {u}close a " + door_name + "{u}.", AWAKEN_CHANCE_MOB_CLOSE_DOOR);
            else core()->message("{u}You see a " + door_name + " close.", AWAKEN_CHANCE_MOB_CLOSE_DOOR);
        }
    }
    
    the_tile->set_ascii(ASCII_DOOR_CLOSED);
    the_tile->set_tags({TileTag::Openable, TileTag::BlocksLight});
    the_tile->clear_tags({TileTag::Closeable, TileTag::Open});
    area->need_fov_recalc();
    timed_action(TIME_CLOSE_DOOR);
}

// Causes the Mobile to die!
void Mobile::die()
{
    const bool unliving = tag(EntityTag::Unliving);
    const bool can_bleed = !tag(EntityTag::ImmunityBleed);

    if (!tag(EntityTag::NoDeathMessage))
    {
        if (is_in_fov()) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + (unliving ? "is destroyed!" : " dies!"),
            AWAKEN_CHANCE_MOB_DEATH_NEAR);
        else if (distance_from(core()->game()->player()) < DEATH_SCREAM_DISTANCE) core()->message("{u}You hear something die nearby!",
            AWAKEN_CHANCE_MOB_DEATH_FAR);
    }

    hp_[0] = 0;
    set_ascii(ASCII_CORPSE);
    if (can_bleed) set_colour(Colour::RED);
    set_name(name(NAME_FLAG_POSSESSIVE) + (unliving ? " remains" : " corpse"));
    if (can_bleed) Gore::splash(x(), y(), GORE_ON_MOBILE_DEATH);
}

// Drops a carried item.
void Mobile::drop_item(uint32_t id)
{
    auto monster = (type() == EntityType::MONSTER ? dynamic_cast<Monster*>(this) : nullptr);

    if (id >= inv()->size()) core()->guru()->halt("Invalid item ID for drop", id, inv()->size());
    if (type() == EntityType::MONSTER && monster->banked_ticks() < TIME_DROP_ITEM) return;
    std::shared_ptr<Entity> item = inv()->at(id);
    core()->game()->area()->entities()->push_back(item);
    inv()->erase(inv()->begin() + id);
    item->set_pos(x(), y());

    if (type() == EntityType::PLAYER) core()->message("You drop {c}" + item->name(NAME_FLAG_THE) + " {w}on the ground.", AWAKEN_CHANCE_DROP_ITEM);
    else if (is_in_fov()) core()->message("{u}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " {u}drops " + item->name(NAME_FLAG_A) +
        " {u}on the ground.", AWAKEN_CHANCE_MOB_DROP_ITEM);
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

// Check if this Mobile is awake and active.
bool Mobile::is_awake() const { return awake_; }

// Checks if this Mobile is dead.
bool Mobile::is_dead() const { return !hp(); }

// Moves in a given direction, or attacks something in the destination tile
bool Mobile::move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy)
{
    auto game = core()->game();
    auto player = game->player();
    auto monster = (type() == EntityType::MONSTER ? std::dynamic_pointer_cast<Monster>(self) : nullptr);

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
        if (!is_player && monster->banked_ticks() < movement_cost) return false;

        if (openable)
        {
            if (is_player) core()->message("You open the " + the_tile->name() + ".");
            else if (area->is_in_fov(xdx, ydy))
            {
                if (is_in_fov()) core()->message("{u}You see " + name(NAME_FLAG_THE) + " {u}open a " + the_tile->name() + "{u}.",
                    AWAKEN_CHANCE_MOB_OPEN_DOOR);
                else core()->message("{u}You see a " + the_tile->name() + " {u}open.", AWAKEN_CHANCE_MOB_OPEN_DOOR);
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
        if (monster) monster->set_last_dir(((dx + 2) << 4) + (dy + 2));
        area->need_fov_recalc();
        game->ui()->redraw_dungeon();

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

        // Check to see if this Actor's feet get covered in blood.
        if (area->tile(self->x(), self->y())->tag(TileTag::Bloodied))
        {
            const int gore_level = Gore::gore_level(x(), y());
            if (bloody_feet() < gore_level)
            {
                add_bloody_feet(Random::rng_float(0, (gore_level > GORE_BLOODY_FEET_MAX ? GORE_BLOODY_FEET_MAX : gore_level)));
                if (bloody_feet() > gore_level) add_bloody_feet(-(bloody_feet() - gore_level));
            }
        }

        // Check to see if we're going to be tracking blood and gore around.
        else
        {
            float gore_dropped = Random::rng_float(0, bloody_feet());
            add_bloody_feet(-gore_dropped);
            if (gore_dropped >= 1) Gore::set_gore(x(), y(), std::round(gore_dropped));
        }

        timed_action(movement_cost);
        return true;
    }
    if (!is_player && monster->banked_ticks() < attack_speed()) return false;
    for (auto entity : *core()->game()->area()->entities())
    {
        if (entity.get() == this) continue; // Ignore ourselves on the list.
        if (!entity->is_at(xdx, ydy)) continue; // Ignore anything not in the target tile.
        if (entity->type() != EntityType::MONSTER && entity->type() != EntityType::PLAYER) continue;    // Ignore anything that isn't a Monster (or the player).

        // This is safe -- we just checked above, only Monster and Player (derived classes of Mobile) can continue to this point in the loop.
        // In case anything could possibly go wrong, dynamic_pointer_cast will either throw an exception or return a null pointer.
        auto mob = std::dynamic_pointer_cast<Mobile>(entity);
        // The dead can't fight back. Okay, that's not strictly true, zombies and skeletons can be pretty feisty, but you know what I mean.
        if (mob->is_dead()) continue;

        timed_action(attack_speed());
        return Combat::bump_attack(self, mob);
    }

    // We're not taking an action right now.
    if (!is_player) monster->clear_banked_ticks();
    return false;
}

// Returns the amount of ticks needed for this Mobile to move one tile.
float Mobile::movement_speed() const { return move_speed_; }

// Retrieves the current or maximum mana points of this Mobile.
uint16_t Mobile::mp(bool max) const { return mp_[max ? 1 : 0]; }

// Sets this Mobile's HP directly.
void Mobile::set_hp(uint16_t current, uint16_t max)
{
    hp_[0] = current;
    if (max < UINT16_MAX) hp_[1] = max;
}

// Sets this Mobile's MP directly.
void Mobile::set_mp(uint16_t current, uint16_t max)
{
    mp_[0] = current;
    if (max < UINT16_MAX) mp_[1] = max;
}

// Sets this Mobile's SP directly.
void Mobile::set_sp(uint16_t current, uint16_t max)
{
    sp_[0] = current;
    if (max < UINT16_MAX) sp_[1] = max;
}

// Retrieves the current or maximum stamina points of this Mobile.
uint16_t Mobile::sp(bool max) const { return sp_[max ? 1 : 0]; }

// Takes damage!
void Mobile::take_damage(int damage)
{
    if (damage <= 0) return;
    if (type() == EntityType::PLAYER) core()->game()->ui()->redraw_stat_bars();
    if (damage >= hp_[0])
    {
        hp_[0] = 0;
        die();
    }
    else hp_[0] -= damage;
}

// Picks up a specified item.
void Mobile::take_item(uint32_t id)
{
    auto area = core()->game()->area();
    auto entities = area->entities();
    auto monster = (type() == EntityType::MONSTER ? dynamic_cast<Monster*>(this) : nullptr);

    if (entities->size() <= id) core()->guru()->halt("Attempt to pick up invalid item ID.", id);
    std::shared_ptr<Entity> entity = entities->at(id);
    if (entity->type() != EntityType::ITEM) core()->guru()->halt("Attempt to pick up non-item entity.", id);
    if (type() != EntityType::PLAYER && monster->banked_ticks() < TIME_TAKE_ITEM) return;

    inventory_add(entity);
    entities->erase(entities->begin() + id);
    if (type() == EntityType::PLAYER) core()->message("You pick up {c}" + entity->name(NAME_FLAG_A) + "{w}.");
    else if (is_in_fov()) core()->message("{u}" + name() + " {u}picks up " + entity->name(NAME_FLAG_A) + "{u}.", AWAKEN_CHANCE_MOB_TAKE_ITEM);
    timed_action(TIME_TAKE_ITEM);
}

// Processes AI for this Mobile each turn.
void Mobile::tick(std::shared_ptr<Entity> self)
{
    Entity::tick(self);
    if (is_dead()) return;
}

// Process slower state-change events that happen less often, such as buffs/debuffs ticking.
void Mobile::tick10(std::shared_ptr<Entity> self)
{
    Entity::tick10(self);
    if (is_dead()) return;

    // Check to see if this Mobile can wake up.
    if (!is_awake() && type() != EntityType::PLAYER)
    {
        auto game = core()->game();
        const bool player_in_los = is_in_fov();
        const float player_distance = game->player()->distance_from(self);
        const Tile* tile = game->area()->tile(x(), y());

        if (tile->id() == TileID::DRUJ_TOMB)
        {
            if (!player_in_los) return; // Druj don't awaken until the player can see them.
            if (Random::rng(1, player_distance) == 1)   // The closer the player gets, the higher the awaken chance.
            {
                wake_message(EnemyWakeMsg::DRUJ_TOMB, false);
                wake();
            }
            // If the first check fails, we'll run another check to give the player a warning that the druj is awakening.
            else if (Random::rng(1, player_distance) == 1) wake_message(EnemyWakeMsg::DRUJ_TOMB, true);
        }
        else
        {
            // Other awakening types are not coded yet. We'll just use a simple wake-on-visual system for a fallback.
            if (player_in_los)
            {
                wake_message(EnemyWakeMsg::NONE, false);
                wake();
            }
        }
    }
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

// Awakens this Mobile, if it's not already.
void Mobile::wake() { awake_ = true; }

// Displays a message when an enemy awakes, or begins to awake.
void Mobile::wake_message(EnemyWakeMsg type, bool warning)
{
    std::string msg;
    switch(type)
    {
        case EnemyWakeMsg::NONE:
            if (!warning) msg = "{r}" + name(NAME_FLAG_CAPITALIZE_FIRST | NAME_FLAG_THE) + " has noticed your presence!";
            break;

        case EnemyWakeMsg::DRUJ_TOMB:
        {
            if (warning)
            {
                switch(Random::rng(5))
                {
                    case 1: msg = "{y}One of the desiccated corpses in its stone tomb starts to slowly move."; break;
                    case 2: msg = "{y}You hear the faint scrape of fingernails against stone."; break;
                    case 3: msg = "{y}Somewhere nearby, parched lungs take in a wheezing breath."; break;
                    case 4: msg = "{y}You hear the creak of ancient bones as something stirs in the darkness."; break;
                    case 5: msg = "{y}For a moment, you thought you saw something move out of the corner of your eye."; break;
                }
            }
            else
            {
                switch(Random::rng(6))
                {
                    case 1: msg = "{r}A bony hand reaches out from the tomb as " + name(NAME_FLAG_THE) + " awakens."; break;
                    case 2: msg = "{r}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " hisses with desiccated lungs as it crawls out of its tomb.";
                        break;
                    case 3: msg = "{r}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST) + " slowly begins to stir, its hollow eyes staring at you."; break;
                    case 4: msg = "{r}With a dull scrape of ancient stone, " + name(NAME_FLAG_THE) + " emerges from its tomb."; break;
                    case 5: msg = "{r}You hear the creaking of parched flesh as " + name(NAME_FLAG_THE) + " begins to move once more."; break;
                    case 6: msg = "{r}" + name(NAME_FLAG_THE | NAME_FLAG_CAPITALIZE_FIRST | NAME_FLAG_POSSESSIVE) + " empty eye sockets suddenly snap open!";
                        break;
                }
            }
        }   // DRUJ_TOMB
    }
    if (msg.size()) core()->message(msg);
}

}   // namespace invictus
