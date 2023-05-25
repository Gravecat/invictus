// gui/nearby.cpp -- The sidebar, showing NPCs and items nearby.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <string>
#include <vector>

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
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
    const int window_w = nearby_window->get_width(), window_h = nearby_window->get_height();
    const int window_midcol = window_w / 2;
    terminal->box(nearby_window, Colour::WHITE);
    terminal->put(Glyph::RTEE, 0, window_h - UI::MESSAGE_LOG_HEIGHT, Colour::WHITE, 0, nearby_window);
    terminal->put(Glyph::BTEE, 0, window_h - 1, Colour::WHITE, 0, nearby_window);

    std::vector<std::string> nearby_lines;
    //bool item_stack_listed = false;
    int current_y = 1;

    terminal->print(" Hostiles Nearby ", window_midcol - 8, 1, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;
    if (current_y >= window_h - 1) return;
    {
        terminal->print("(nothing visible)", window_midcol - 8, current_y, Colour::BLACK_BOLD, 0, nearby_window);
        current_y++;
    }
    current_y++;

    if (current_y >= window_h - 1) return;
    terminal->print(" Items Nearby ", window_midcol - 7, current_y, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;
    if (current_y >= window_h - 1) return;
    {
        terminal->print("(nothing visible)", window_midcol - 8, current_y, Colour::BLACK_BOLD, 0, nearby_window);
        current_y++;
    }
    current_y++;

    std::vector<Tile*> tiles;
    int visible_x = ui->dungeon_view()->get_width(), visible_y = ui->dungeon_view()->get_height();
    auto area = core()->game()->area();
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
                if (tc->is_identical_to(tile))
                {
                    found = true;
                    break;
                }
            }
            if (!found) tiles.push_back(tile);
        }
    }

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
    terminal->print(" Other Symbols ", window_midcol - 7, current_y, Colour::WHITE, PRINT_FLAG_REVERSE, nearby_window);
    current_y++;

    for (auto tile : tiles)
    {
        if (current_y >= window_h - 1) return;
        terminal->put(tile->ascii(), 2, current_y, tile->colour(), 0, nearby_window);
        
        std::vector<std::string> name_str = StrX::string_explode_colour("{w}" + tile->name(), 16);
        for (auto l : name_str)
        {
            terminal->print(l, 4, current_y, Colour::WHITE, 0, nearby_window);
            current_y++;
        }
    }
    if (current_y >= window_h - 1) return;
    terminal->put(player->ascii(), 2, current_y, player->colour(), 0, nearby_window);
    terminal->print("you!", 4, current_y, Colour::WHITE, 0, nearby_window);
}

}   // namespace invictus
