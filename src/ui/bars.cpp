// ui/bars.cpp -- UI code to render health, stamina, mana and experience bars.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/bars.hpp"
#include "ui/ui.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Renders a health/mana/etc. bar.
void Bars::render_bar(int x, int y, unsigned int width, const std::string &name, float value, float value_max, Colour bar_colour, bool numbers, bool round_up,
    std::shared_ptr<Window> win)
{
    auto terminal = core()->terminal();

    unsigned int bar_width = static_cast<int>(std::round((value / value_max) * width));
    if (value > 0 && !bar_width && round_up) bar_width = 1;
    std::string msg = name;
    if (numbers) msg += ": " + StrX::ftos(round_up ? std::ceil(value) : std::round(value)) + "/" + StrX::ftos(round_up ? std::ceil(value_max) :
        std::round(value_max));
    if (msg.size() < width)
    {
        int excess = width - msg.size();
        int left_padding = excess / 2;
        msg = std::string(left_padding, ' ') + msg + std::string(excess - left_padding, ' ');
    }
    std::string msg_left = msg.substr(0, bar_width);
    std::string msg_right = msg.substr(bar_width);
    terminal->print(msg_left, x, y, bar_colour, PRINT_FLAG_REVERSE | PRINT_FLAG_BOLD, win);
    terminal->print(msg_right, x + msg_left.size(), y, Colour::WHITE_BOLD, 0, win);
}

// Renders the player's health and mana bars.
void Bars::render_health_mana_bars()
{
    auto terminal = core()->terminal();
    auto ui = core()->game()->ui();
    auto window = ui->stat_bars();
    int window_w = window->get_width();

    const int hp_bar_width = window_w / 2;
    const int sp_bar_width = (window_w - hp_bar_width) / 2;
    const int mp_bar_width = window_w - hp_bar_width - sp_bar_width;

    render_bar(0, 0, hp_bar_width, "HP", 72, 100, Colour::RED_WHITE, true, true, window);
    render_bar(hp_bar_width, 0, sp_bar_width, "SP", 35, 100, Colour::GREEN_WHITE, true, false, window);
    render_bar(hp_bar_width + sp_bar_width, 0, mp_bar_width, "MP", 80, 100, Colour::BLUE_WHITE, true, false, window);
    render_bar(0, 1, window_w, "Experience", 720, 1000, Colour::MAGENTA_WHITE, true, false, window);
}

}   // namespace invictus
