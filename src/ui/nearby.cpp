// gui/nearby.cpp -- The sidebar, showing NPCs and items nearby.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <algorithm>
#include <string>
#include <vector>

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "entity/item.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/message-log.hpp"
#include "ui/bars.hpp"
#include "ui/nearby.hpp"
#include "ui/ui.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Renders the Nearby bar.
void Nearby::render()
{
    auto terminal = core()->terminal();
    auto player = core()->game()->player();
    auto ui = core()->game()->ui();
    auto nearby_window = ui->nearby_window();
    auto area = core()->game()->area();
    const int window_w = nearby_window->get_width(), window_h = nearby_window->get_height();
    terminal->box(nearby_window, Colour::WHITE);
    terminal->put(Glyph::RTEE, 0, window_h - MESSAGE_LOG_HEIGHT, Colour::WHITE, 0, nearby_window);
    terminal->put(Glyph::BTEE, 0, window_h - 1, Colour::WHITE, 0, nearby_window);

    std::vector<std::string> nearby_lines;
    bool item_stack_listed = false;
    std::vector<std::shared_ptr<Entity>> mobiles, items;

    for (auto entity : *area->entities())
    {
        if (!entity->is_in_fov() || entity->is_at(player->x(), player->y())) continue;
        auto entity_type = entity->type();
        if (entity_type != EntityType::ITEM && entity_type != EntityType::MOBILE) continue;
        bool is_item = false;
        if (entity_type == EntityType::MOBILE && std::dynamic_pointer_cast<Mobile>(entity)->is_dead()) is_item = true;
        else if (entity_type == EntityType::ITEM) is_item = true;
        if (is_item)
        {
            bool already_on_list = false;
            bool is_stack = area->is_item_stack(entity->x(), entity->y());
            for (auto item : items)
            {
                if (item->name() == entity->name() && !is_stack)
                {
                    already_on_list = true;
                    break;
                }
            }
            if (!already_on_list)
            {
                if (is_stack)
                {
                    if (!item_stack_listed)
                    {
                        item_stack_listed = true;
                        auto stack = std::make_shared<Item>();
                        stack->set_name("multiple items");
                        stack->set_ascii(ASCII_STACK);
                        stack->set_colour(Colour::MAGENTA);
                        items.push_back(stack);
                    }
                }
                else items.push_back(entity);
            }
        }
        else mobiles.push_back(entity);
    }

    auto sort_entities = [](const std::shared_ptr<Entity> &lhs, const std::shared_ptr<Entity> &rhs) -> bool
    {
        return lhs->distance_from(core()->game()->player()) < rhs->distance_from(core()->game()->player());
    };

    if (mobiles.size()) std::sort(mobiles.begin(), mobiles.end(), sort_entities);
    if (items.size()) std::sort(items.begin(), items.end(), sort_entities);

    int current_y = 1;
    terminal->print(StrX::pad_string_center("Hostiles Nearby", window_w - 4), 2, 1, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;
    if (current_y >= window_h - 1) return;
    if (mobiles.size())
    {
        for (auto mob : mobiles)
        {
            terminal->put(mob->ascii(), 2, current_y, mob->colour(), 0, nearby_window);

            auto mob_ptr = std::dynamic_pointer_cast<Mobile>(mob);
            std::string mob_name = mob->name();
            if (static_cast<int>(mob_name.size()) > window_w - 6) mob_name = mob_name.substr(0, window_w - 6);
            Colour bar_col = Colour::RED_WHITE;
            if (!mob_ptr->is_awake()) bar_col = Colour::BLUE_WHITE;
            Bars::render_bar(4, current_y, window_w - 6, mob_name, mob_ptr->hp(), mob_ptr->hp(true), bar_col, BAR_FLAG_ROUND_UP, nearby_window);
            if (++current_y >= window_h - 1) return;
        }
    }
    else
    {
        terminal->print("(nothing visible)", 2, current_y, Colour::BLACK_BOLD, 0, nearby_window);
        current_y++;
    }
    current_y++;

    if (current_y >= window_h - 1) return;
    terminal->print(StrX::pad_string_center("Items Nearby", window_w - 4), 2, current_y, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;
    if (current_y >= window_h - 1) return;
    if (items.size())
    {
        for (auto item : items)
        {
            terminal->put(item->ascii(), 2, current_y, item->colour(), 0, nearby_window);
            std::vector<std::string> name_str = StrX::string_explode_colour("{w}" + item->name(), 16);
            for (auto l : name_str)
            {
                if (current_y >= window_h - 1) return;
                terminal->print(l, 4, current_y, Colour::WHITE, 0, nearby_window);
                current_y++;
            }
        }
    }
    else
    {
        terminal->print("(nothing visible)", 2, current_y, Colour::BLACK_BOLD, 0, nearby_window);
        current_y++;
    }
    current_y++;

    std::vector<Tile*> tiles;
    int visible_x = ui->dungeon_view()->get_width(), visible_y = ui->dungeon_view()->get_height();
    for (int x = 0; x < area->width(); x++)
    {
        int ox = x - area->offset_x();
        if (ox < 0 || ox >= visible_x) continue;
        for (int y = 0; y < area->height(); y++)
        {
            if (x == player->x() && y == player->y()) continue;
            int oy = y - area->offset_y();
            if (oy < 0 || oy >= visible_y) continue;

            Tile* tile = area->tile(x, y);
            bool is_visible = area->is_in_fov(x, y);
            if (!is_visible) continue;

            bool found = false;
            for (auto tc : tiles)
            {
                if (tc->is_identical_to(tile, true))
                {
                    found = true;
                    break;
                }
            }
            if (!found) tiles.push_back(tile);
        }
    }

    // Add a fake 'tile' for the player.
    std::shared_ptr<Tile> fake_player_tile = std::make_shared<Tile>();
    fake_player_tile->set_ascii(player->ascii());
    fake_player_tile->set_colour(player->colour());
    fake_player_tile->set_name("you!");
    // okay okay yes, putting a raw pointer from a shared_ptr in here is a kinda bad idea, but it's only for the scope of this method, so it's safe enough.
    tiles.push_back(fake_player_tile.get());

    bool rearranged = false;
    do
    {
        rearranged = false;
        for (unsigned int i = 0; i < tiles.size() - 1; i++)
        {
            if (tiles.at(i)->name() > tiles.at(i + 1)->name())
            {
                rearranged = true;
                std::iter_swap(tiles.begin() + i, tiles.begin() + i + 1);
                break;
            }
        }
    } while(rearranged);

    if (current_y >= window_h - 1) return;
    terminal->print(StrX::pad_string_center("Other Symbols", window_w - 4), 2, current_y, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;

    for (auto tile : tiles)
    {
        if (current_y >= window_h - 1) return;
        terminal->put(tile->ascii(true), 2, current_y, tile->colour(true), 0, nearby_window);
        
        std::vector<std::string> name_str = StrX::string_explode_colour("{w}" + tile->name(), 16);
        for (auto l : name_str)
        {
            terminal->print(l, 4, current_y, Colour::WHITE, 0, nearby_window);
            current_y++;
        }
    }
}

}   // namespace invictus
