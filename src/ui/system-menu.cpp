// ui/system-menu.cpp -- The main system menu, giving the player options such as saving or quitting the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>

#include "core/core.hpp"
#include "ui/menu.hpp"
#include "ui/system-menu.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Opens the system menu.
void SystemMenu::open()
{
    auto menu = std::make_unique<Menu>();
    menu->set_title("System Menu");

    menu->add_item("{R}Quit (Don't Save)");

    int result = menu->render();
    switch(result)
    {
        case 0:
            if (UI::are_you_sure())
            {
                core()->cleanup();
                exit(EXIT_SUCCESS);
            }
            break;
    }
}

}   // namespace invictus
