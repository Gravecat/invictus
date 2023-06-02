// ui/bars.cpp -- UI code to render various forms of status bars.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "core/core.hpp"
#include "terminal/terminal.hpp"
#include "ui/bars.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Renders a coloured bar.
void Bars::render_bar(int x, int y, unsigned int width, const std::string &name, float value, float value_max, Colour bar_colour, int flags,
    std::shared_ptr<Window> win)
{
    auto terminal = core()->terminal();

    const bool numbers = ((flags & BAR_FLAG_NUMBERS) == BAR_FLAG_NUMBERS);
    const bool round_up = ((flags & BAR_FLAG_ROUND_UP) == BAR_FLAG_ROUND_UP);
    const bool percentage = ((flags & BAR_FLAG_PERCENTAGE) == BAR_FLAG_PERCENTAGE);

    unsigned int bar_width = static_cast<int>(std::round((value / value_max) * width));
    if (bar_width > width) bar_width = width;
    if (value > 0 && !bar_width && round_up) bar_width = 1;
    std::string msg = name;
    if (percentage)
    {
        const float perc = (value / value_max) * 100.0f;
        msg += ": " + StrX::ftos(round_up ? std::ceil(perc) : std::round(perc)) + "%";
    }
    else if (numbers)
        msg += ": " + StrX::ftos(round_up ? std::ceil(value) : std::round(value)) + "/" + StrX::ftos(round_up ? std::ceil(value_max) : std::round(value_max));

    if (msg.size() > width)
    {
        if (percentage)
        {
            const float perc = (value / value_max) * 100.0f;
            msg = StrX::ftos(round_up ? std::ceil(perc) : std::round(perc)) + "%";
        }
        else if (numbers)
        {
            msg = StrX::ftos(round_up ? std::ceil(value) : std::round(value)) + "/" + StrX::ftos(round_up ? std::ceil(value_max) : std::round(value_max));
            if (msg.size() > width) msg = StrX::ftos(round_up ? std::ceil(value) : std::round(value));
        }
    }
    if (msg.size() < width)
    {
        int excess = width - msg.size();
        int left_padding = excess / 2;
        msg = std::string(left_padding, ' ') + msg + std::string(excess - left_padding, ' ');
    }
    std::string msg_left = msg.substr(0, bar_width);
    std::string msg_right = msg.substr(bar_width);
    terminal->print(msg_left, x, y, bar_colour, PRINT_FLAG_REVERSE | PRINT_FLAG_BOLD, win);
    terminal->print(msg_right, x + msg_left.size(), y, Colour::BLACK_WHITE, PRINT_FLAG_REVERSE | PRINT_FLAG_BOLD, win);
}

}   // namespace invictus
