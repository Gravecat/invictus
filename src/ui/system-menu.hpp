// ui/system-menu.hpp -- The main system menu, giving the player options such as saving or quitting the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_SYSTEM_MENU_HPP_
#define UI_SYSTEM_MENU_HPP_

namespace invictus
{

class SystemMenu
{
public:
    static void open(); // Opens the system menu.

private:
    static void about();        // Displays the version info.
    static void quit_no_save(); // Quits the game without saving.
};

}       // namespae invictus
#endif  // UI_SYSTEM_MENU_HPP_
