// entity/player.cpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use the same stats for combat etc.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "entity/item.hpp"
#include "entity/player.hpp"
#include "tune/ascii-symbols.hpp"
#include "terminal/terminal.hpp"
#include "tune/fov-lighting.hpp"
#include "ui/menu.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor.
Player::Player() : Mobile()
{
    set_ascii(ASCII_PLAYER);
    set_colour(Colour::WHITE_BOLD);
    set_light_power(4);
    set_name("player");
}

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
    core()->message("{C}Which direction? (Direction key to choose, cancel key to abort.)");
    while (true)
    {
        core()->game()->ui()->redraw_message_log();
        core()->game()->ui()->render();
        key = core()->terminal()->get_key();
        switch(key)
        {
            case 'k': case Key::ARROW_UP: *dy = -1; return;             // North
            case 'j': case Key::ARROW_DOWN: *dy = 1; return;            // South
            case 'h': case Key::ARROW_LEFT: *dx = -1; return;           // West
            case 'l': case Key::ARROW_RIGHT: *dx = 1; return;           // East
            case 'b': case Key::END: *dx = -1; *dy = 1; return;         // Southwest
            case 'n': case Key::PAGE_DOWN: *dx = 1; *dy = 1; return;    // Southeast
            case 'y': case Key::HOME: *dx = -1; *dy = -1; return;       // Northwest
            case 'u': case Key::PAGE_UP: *dx = 1; *dy = -1; return;     // Northeast
        }
    }
    core()->message("{C}Selection cancelled.");
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
    if (items_nearby.size() == 1)
    {
        take_item(items_nearby.at(0));
        return;
    }
}

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

// Interact with carried items.
void Player::take_inventory()
{
    if (!inv()->size())
    {
        core()->message("{y}You are carrying nothing.");
        return;
    }

    auto inv_menu = std::make_shared<Menu>();
    
    inv_menu->set_title("Inventory");
    for (auto item : *inv())
        inv_menu->add_item(item->name(), item->ascii(), item->colour());
    inv_menu->render();
}

}   // namespace invictus
