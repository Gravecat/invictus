// ui/menu.hpp -- Menu class definition, for a fairly generic scrollable menu of items.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_MENU_HPP_
#define UI_MENU_HPP_

#include <memory>
#include <string>
#include <vector>

#include "terminal/terminal-shared-defs.hpp"


namespace invictus
{

class Window;   // defined in terminal/window.hpp


class Menu
{
public:
                    Menu(int fx = -1, int fy = -1);     // Constructor.
    void            add_item(const std::string &txt, int ch = '\0', Colour col = Colour::WHITE, bool arrow = false);    // Adds an item to this Menu.
    void            add_item(const std::string &txt, bool arrow);   // Another option to specify an arrow without all the other stuff.
    unsigned int    get_size();                         // Checks how many items are in this Menu already.
    int             render();                           // Renders the menu, returns the chosen menu item (or -1 if none chosen).
    void            set_highlight(bool highlight);      // Sets the Menu option highlighting on or off.
    void            set_selected(unsigned int pos);     // Sets the currently-selected item.
    void            set_title(const std::string &str);  // Sets the Menu's title.

private:
    void            reposition();   // Repositions the menu.

    bool                        allow_highlight_;       // Do we want the currently-selected option to be highlighted?
    std::vector<bool>           arrows_;                // Whether the menu items have a > arrow to their side.
    std::vector<Colour>         colour_;                // The colours of menu items.
    int                         fixed_x_, fixed_y_;     // The fixed X,Y coordinates, if specified.
    std::vector<int>            item_chars_;            // Extra characters for menu items.
    std::vector<int>            item_x_;                // The menu item positions.
    std::vector<std::string>    items_;                 // The menu item text.
    unsigned int                offset_;                // The menu scroll.
    int                         pos_x_, pos_y_;         // Screen coordinates.
    bool                        return_after_render_;   // Whether to return the selected value *after* doing one more render pass.
    unsigned int                selected_;              // The selected menu item.
    int                         size_x_, size_y_;       // The size of the menu.
    std::string                 title_;                 // The menu's title, if any.
    int                         title_x_;               // Title horizontal position.
    std::shared_ptr<Window>     window_;                // The Menu's rendering window.
};

}       // namespace invictus
#endif  // UI_MENU_HPP_
