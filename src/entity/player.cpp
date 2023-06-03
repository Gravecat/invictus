// entity/player.cpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use some of the same code.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/item.hpp"
#include "entity/player.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/combat.hpp"
#include "terminal/terminal.hpp"
#include "tune/fov-lighting.hpp"
#include "ui/menu.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor.
Player::Player() : Mobile(), finesse_(2), intellect_(1), might_(2)
{
    set_ascii(ASCII_PLAYER);
    set_colour(Colour::WHITE_BOLD);
    set_light_power(4);
    set_name("player");
    recalc_max_hp_mp_sp();
    set_hp_regen_speed(PLAYER_HP_REGEN_SPEED);
    wake();
}

// Returns the total armour modifier from this Player and their equipped gear.
int Player::armour()
{
    auto armour_item = equipment(EquipSlot::BODY);
    int armour_value = armour_item->armour();
    int armour_value_might = std::max(10, armour_value) + (might_ * ARMOUR_PER_MIGHT);
    if (armour_item->tag(EntityTag::ArmourLight))
    {
        int armour_value_finesse = armour_value + (finesse_ * DODGE_PER_FINESSE);
        return std::max(armour_value_finesse, armour_value_might);
    }
    else return armour_value_might;
}

// Checks if this Player blocks a specified tile.
bool Player::blocks_tile(int x_tile, int y_tile) const { return (x_tile == x() && y_tile == y()); }

// Attempts to close a nearby door.
void Player::close_a_door()
{
    auto area = core()->game()->area();
    int dx = 0, dy = 0, doors_nearby = 0;
    for (int tx = -1; tx <= 1; tx++)
    {
        for (int ty = -1; ty <= 1; ty++)
        {
            if (tx == 0 && ty == 0) continue;
            if (area->tile(x() + tx, y() + ty)->tag(TileTag::Closeable))
            {
                dx = tx;
                dy = ty;
                doors_nearby++;
            }
        }
    }

    if (!doors_nearby)
    {
        core()->message("{Y}There's nothing nearby that you can close.");
        return;
    }
    else if (doors_nearby > 1)
    {
        get_direction(&dx, &dy);
        if (!dx && !dy) return;
    }

    auto target_tile = area->tile(x() + dx, y() + dy);
    if (!target_tile->tag(TileTag::Closeable))
    {
        core()->message("{Y}That isn't something you can close.");
        return;
    }
    close_door(x() + dx, y() + dy);
    core()->game()->ui()->redraw_dungeon();
}

// Returns this Player's dodge score.
int Player::dodge()
{
    const int base_dodge = 10 + (finesse_ * DODGE_PER_FINESSE);
    auto armour_item = equipment(EquipSlot::BODY);

    if (armour_item->item_type() == ItemType::NONE || armour_item->tag(EntityTag::ArmourLight)) return base_dodge;
    else if (armour_item->tag(EntityTag::ArmourHeavy)) return std::min(base_dodge, 10);
    else if (armour_item->tag(EntityTag::ArmourMedium)) return std::min(armour_item->max_finesse(), base_dodge);
    else throw std::runtime_error("Unable to determine armour type for " + name());
}

// Retrieves this Player's finesse attribute.
int8_t Player::finesse() const { return finesse_; }

// Calculates the player's field-of-view radius.
uint16_t Player::fov_radius() const
{
    const int lp = light_power();
    if (lp >= PLAYER_FOV_LIGHT_HIGH) return lp + PLAYER_FOV_BONUS_HIGH;
    else if (lp >= PLAYER_FOV_LIGHT_MEDIUM) return lp + PLAYER_FOV_BONUS_MEDIUM;
    else if (!lp) return PLAYER_FOV_MINIMUM;
    else return std::max<int>(lp + PLAYER_FOV_BONUS_LOW, PLAYER_FOV_MINIMUM);
}

// Gets a direction from the player.
void Player::get_direction(int *dx, int *dy) const
{
    *dx = *dy = 0;
    int key = 0;
    auto game = core()->game();
    core()->message("{C}Which direction? (Direction key to choose, cancel key to abort.)");
    while (true)
    {
        game->ui()->redraw_message_log();
        game->ui()->render();
        key = game->get_key();
        if (game->is_key_north(key)) { *dy = -1; return; }
        else if (game->is_key_south(key)) { *dy = 1; return; }
        else if (game->is_key_east(key)) { *dx = 1; return; }
        else if (game->is_key_west(key)) { *dx = -1; return; }
        else if (game->is_key_northeast(key)) { *dx = 1; *dy = -1; return; }
        else if (game->is_key_northwest(key)) { *dx = -1; *dy = -1; return; }
        else if (game->is_key_southeast(key)) { *dx = 1; *dy = 1; return; }
        else if (game->is_key_southwest(key)) { *dx = -1; *dy = 1; return; }
        else if (key == Key::RESIZE || !key) { }
        else { core()->message("{C}Selection cancelled."); return; }
    }
}

// Picks something up off the ground.
void Player::get_item()
{
    auto area = core()->game()->area();
    std::vector<uint32_t> items_nearby;
    for (unsigned int i = 0; i < area->entities()->size(); i++)
    {
        auto entity = area->entities()->at(i);
        if (entity->type() != EntityType::ITEM) continue;
        if (entity->is_at(x(), y())) items_nearby.push_back(i);
    }
    if (!items_nearby.size()) core()->message("{y}There isn't anything you can pick up here.");
    else if (items_nearby.size() == 1)
    {
        take_item(items_nearby.at(0));
        return;
    }
    else ground_items();
}

// Interact with items on the ground.
void Player::ground_items()
{
    auto area = core()->game()->area();
    std::vector<uint32_t> items_nearby;
    for (unsigned int i = 0; i < area->entities()->size(); i++)
    {
        auto entity = area->entities()->at(i);
        if (entity->type() != EntityType::ITEM) continue;
        if (entity->is_at(x(), y())) items_nearby.push_back(i);
    }
    if (!items_nearby.size()) core()->message("{y}There's nothing to interact with here.");

    auto items_menu = std::make_unique<Menu>();
    items_menu->set_title("Nearby Items");
    for (auto id : items_nearby)
    {
        auto entity = area->entities()->at(id);
        items_menu->add_item(entity->name(), entity->ascii(), entity->colour(), true);
    }
    int result = items_menu->render();
    if (result >= 0 && result < static_cast<int>(items_nearby.size())) item_interaction(items_nearby.at(result), ItemLocation::GROUND);
}

// Retrieves this Player's intellect attribute.
int8_t Player::intellect() const { return intellect_; }

// Interacts with an item.
void Player::item_interaction(uint32_t id, ItemLocation loc)
{
    std::shared_ptr<Entity> entity = nullptr;
    std::shared_ptr<Item> item = nullptr;

    switch(loc)
    {
        case ItemLocation::INVENTORY:
            if (id >= inv()->size()) core()->guru()->halt("Invalid item interaction ID", id, inv()->size());
            entity = inv()->at(id);
            break;
        case ItemLocation::EQUIPMENT:
            if (id >= static_cast<unsigned int>(EquipSlot::_END)) core()->guru()->halt("Invalid item interaction ID", id, equ()->size());
            entity = equ()->at(id);
            break;
        case ItemLocation::GROUND:
        {
            auto area = core()->game()->area();
            if (area->entities()->size() <= id) core()->guru()->halt("Invalid item interaction ID", id, area->entities()->size());
            entity = area->entities()->at(id);
            break;
        }
    }
    if (!entity) core()->guru()->halt("Null item interaction pointer", id);
    else if (entity->type() != EntityType::ITEM) core()->guru()->halt("Invalid item interaction target", id);
    item = std::dynamic_pointer_cast<Item>(entity);
    
    auto item_menu = std::make_unique<Menu>();
    item_menu->set_title(item->name());
    item_menu->add_item("Do Nothing");
    std::vector<ItemInteraction> interactions;
    interactions.push_back(ItemInteraction::DO_NOTHING);

    switch(loc)
    {
        case ItemLocation::INVENTORY:
            item_menu->add_item("Drop");
            interactions.push_back(ItemInteraction::DROP);

            if (item->item_type() == ItemType::WEAPON || item->item_type() == ItemType::ARMOUR || item->item_type() == ItemType::SHIELD)
            {
                item_menu->add_item("Equip");
                interactions.push_back(ItemInteraction::EQUIP);
            }

            break;
        case ItemLocation::GROUND:
            item_menu->add_item("Take");
            interactions.push_back(ItemInteraction::TAKE);
            break;
        case ItemLocation::EQUIPMENT:
            item_menu->add_item("Unequip");
            interactions.push_back(ItemInteraction::UNEQUIP);
            break;
        default: break;
    }

    int choice = item_menu->render();
    if (choice >= 0 && choice < static_cast<int>(interactions.size())) switch(interactions.at(choice))
    {
        case ItemInteraction::DO_NOTHING: return;
        case ItemInteraction::DROP: drop_item(id); break;
        case ItemInteraction::EQUIP: equip_item(id); break;
        case ItemInteraction::TAKE: take_item(id); break;
        case ItemInteraction::UNEQUIP: unequip_item(static_cast<EquipSlot>(id)); break;
    }
}

// Retrieves this Player's might attribute.
int8_t Player::might() const { return might_; }

// Attempts to open a nearby door.
void Player::open_a_door()
{
    auto area = core()->game()->area();
    int dx = 0, dy = 0, doors_nearby = 0;
    for (int tx = -1; tx <= 1; tx++)
    {
        for (int ty = -1; ty <= 1; ty++)
        {
            if (tx == 0 && ty == 0) continue;
            if (area->tile(x() + tx, y() + ty)->tag(TileTag::Openable))
            {
                dx = tx;
                dy = ty;
                doors_nearby++;
            }
        }
    }

    if (!doors_nearby)
    {
        core()->message("{Y}There's nothing nearby that you can open.");
        return;
    }
    else if (doors_nearby > 1)
    {
        get_direction(&dx, &dy);
        if (!dx && !dy) return;
    }

    auto target_tile = area->tile(x() + dx, y() + dy);
    if (!target_tile->tag(TileTag::Openable))
    {
        core()->message("{Y}That isn't something you can open.");
        return;
    }
    move_or_attack(core()->game()->player(), dx, dy);
    core()->game()->ui()->redraw_dungeon();
}

// Recalculates the maximum HP/SP/MP values, based on Strength, Finesse and Intellect.
void Player::recalc_max_hp_mp_sp()
{
    const int new_hp = BASE_HIT_POINTS + (might_ * HIT_POINTS_PER_MIGHT);
    const int new_sp = BASE_STAMINA_POINTS + (finesse_ * STAMINA_PER_FINESSE) + (might_ * STAMINA_PER_MIGHT);
    const int new_mp = BASE_MANA_POINTS + (intellect_ * MANA_PER_INTELLECT);

    set_hp(new_hp, new_hp);
    set_sp(new_sp, new_sp);
    set_mp(new_mp, new_mp);
}

// Sets this Mobile's Finesse level.
void Player::set_finesse(int8_t new_fin) { finesse_ = new_fin; }

// Interact with carried items.
void Player::take_inventory(bool equipment)
{
    if (!equipment && !inv()->size())
    {
        core()->message("{y}You are carrying nothing.");
        return;
    }

    if (equipment)
    {
        auto equ_menu = std::make_unique<Menu>();
        equ_menu->set_title("Equipment");
        equ_menu->left_aligned(true);
        bool has_gear = false;
        std::shared_ptr<Item> main_hand_item = nullptr;
        for (unsigned int i = 0; i < static_cast<unsigned int>(EquipSlot::_END); i++)
        {
            std::string slot_name, line_str;
            switch(static_cast<EquipSlot>(i))
            {
                case EquipSlot::HAND_MAIN: slot_name = "in main hand"; break;
                case EquipSlot::HAND_OFF: slot_name = "in off hand"; break;
                case EquipSlot::BODY: slot_name = "worn on body"; break;
                case EquipSlot::HEAD: slot_name = "worn on head"; break;
                case EquipSlot::HANDS: slot_name = "worn on hands"; break;
                case EquipSlot::FEET: slot_name = "worn on feet"; break;
                case EquipSlot::_END: break;    // This can't happen, but this line keeps the compiler happy.
            }
            auto item = equ()->at(i);
            if (item->item_type() == ItemType::NONE)
            {
                if (static_cast<EquipSlot>(i) == EquipSlot::HAND_OFF && main_hand_item && main_hand_item->item_type() != ItemType::NONE &&
                    (main_hand_item->tag(EntityTag::HandAndAHalf) || main_hand_item->tag(EntityTag::TwoHanded)))
                    equ_menu->add_item("{B}(" + main_hand_item->name() + ") (in off hand)");
                else equ_menu->add_item("{B}(nothing " + slot_name + ")", false);
            }
            else
            {
                if (static_cast<EquipSlot>(i) == EquipSlot::HAND_MAIN) main_hand_item = item;
                equ_menu->add_item(item->name() + " {B}(" + slot_name + ")", item->ascii(), item->colour(), true);
                has_gear = true;
            }
        }
        if (!has_gear) equ_menu->set_highlight(false);
        int result = equ_menu->render();
        if (has_gear && result >= 0 && result < static_cast<int>(EquipSlot::_END) && equ()->at(result)->item_type() != ItemType::NONE)
            item_interaction(result, ItemLocation::EQUIPMENT);
    }
    else
    {
        auto inv_menu = std::make_unique<Menu>();
        inv_menu->set_title("Inventory");
        inv_menu->left_aligned(true);
        for (auto item : *inv())
            inv_menu->add_item(item->name(), item->ascii(), item->colour(), true);
        int result = inv_menu->render();
        if (result >= 0) item_interaction(result, ItemLocation::INVENTORY);
    }
}

// Sets this Player's Intellect attribute.
void Player::set_intellect(int8_t new_int) { intellect_ = new_int; }

// Sets this Player's Might attribute.
void Player::set_might(int8_t new_mig) { might_ = new_mig; }

// This Player has made an action which takes time.
void Player::timed_action(float time_taken) { core()->game()->pass_time(time_taken); }

}   // namespace invictus
