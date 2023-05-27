// ui/bars.hpp -- UI code to render various forms of status bars.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_BARS_HPP_
#define UI_BARS_HPP_

#include <memory>
#include <string>


namespace invictus
{

enum class Colour : uint8_t;    // defined in terminal/terminal-shared-defs.hpp

class Window;   // defined in terminal/window.hpp


constexpr int   BAR_FLAG_NUMBERS =      1;  // Display numbers on the bar?
constexpr int   BAR_FLAG_ROUND_UP =     2;  // Always round the bar size and numbers up, rather than rounding normally?
constexpr int   BAR_FLAG_PERCENTAGE =   4;  // Display the bar numbers as a percentage (ignores BAR_FLAG_NUMBERS).


class Bars
{
public:
    static void render_bar(int x, int y, unsigned int width, const std::string &name, float value, float value_max, Colour bar_colour, int flags = 0,
        std::shared_ptr<Window> win = nullptr); // Renders a coloured bar.
};

}       // namespace invictus
#endif  // UI_BARS_HPP_
