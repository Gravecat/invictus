// ui/system-menu.cpp -- The main system menu, giving the player options such as saving or quitting the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/save-load.hpp"
#include "core/version.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/menu.hpp"
#include "ui/system-menu.hpp"
#include "ui/ui.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Displays the version info.
void SystemMenu::about()
{
    std::vector<std::string> lines;
    lines.push_back("{g}.~{r}* {R}Morior Invictus {r}*{g}~.");
    lines.push_back("{r}" + INVICTUS_VERSION_STRING);
    lines.push_back("");
    lines.push_back("{g}Copyright (c) 2023 Raine \"Gravecat\" Simmons");
    lines.push_back("");
    lines.push_back("{U}github.com/Gravecat/invictus");
    unsigned int longest = StrX::center_strvec(lines);

    auto win = std::make_shared<Window>(longest + 4, lines.size() + 4);
    auto terminal = core()->terminal();
    terminal->box(win);
    terminal->print(" About ", win->get_width() / 2 - 3, 0, Colour::WHITE, PRINT_FLAG_REVERSE, win);
    for (unsigned int i = 0; i < lines.size(); i++)
        terminal->print(lines.at(i), 2, 2 + i, Colour::WHITE, 0, win);
    win->move(terminal->get_midcol() - win->get_width() / 2, terminal->get_midrow() - win->get_height() / 2);
    terminal->flip();
    if (terminal->get_key() == Key::RESIZE) core()->game()->ui()->window_resized();
}

// Opens the system menu.
void SystemMenu::open()
{
    auto menu = std::make_unique<Menu>();
    menu->set_title("System Menu");

    menu->add_item("About");
    menu->add_item("Save Game");
    menu->add_item("Save & Quit");
    menu->add_item("{R}Quit (Don't Save)");

    int result = menu->render();
    switch(result)
    {
        case 0: about(); break;
        case 1: SaveLoad::save_game(); break;
        case 2: save_and_quit(); break;
        case 3: quit_no_save(); break;
    }
}

// Quits the game without saving.
void SystemMenu::quit_no_save()
{
    int result = core()->game()->ui()->yes_no("This will lose any progress you have made since your last save. Are you sure?");
    if (result != 'Y') return;
    core()->cleanup();
    exit(EXIT_SUCCESS);
}

// Saves the game, then closes.
void SystemMenu::save_and_quit()
{
    SaveLoad::save_game();
    core()->cleanup();
    exit(EXIT_SUCCESS);
}

}   // namespace invictus
