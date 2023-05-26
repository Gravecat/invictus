// ui/bars.hpp -- UI code to render health, stamina, mana and experience bars.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_BARS_HPP_
#define UI_BARS_HPP_

#include <memory>
#include <string>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp

class Window;   // defined in terminal/window.hpp


class Bars
{
public:
    static void render_bar(int x, int y, unsigned int width, const std::string &name, float value, float value_max, Colour bar_colour, bool numbers = true,
        bool round_up = false, std::shared_ptr<Window> win = nullptr);  // Renders a health/mana/etc. bar.
    static void render_health_mana_bars();  // Renders the player's health and mana bars.
};

}       // namespace invictus
#endif  // UI_BARS_HPP_
