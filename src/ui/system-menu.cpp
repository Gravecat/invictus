// ui/system-menu.cpp -- The main system menu, giving the player options such as saving or quitting the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

#include "core/core.hpp"
#include "core/game-manager.hpp"
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
    lines.push_back("{r}.~* {R}Morior Invictus {r}*~.");
    lines.push_back("{r}" + INVICTUS_VERSION_STRING);
    lines.push_back("");
    lines.push_back("{Y}Copyright (c) 2023 Raine \"Gravecat\" Simmons");
    lines.push_back("");
    lines.push_back("{y}github.com/Gravecat/invictus");
    unsigned int longest = StrX::center_strvec(lines);

    auto win = std::make_shared<Window>(longest + 4, lines.size() + 4);
    auto terminal = core()->terminal();
    terminal->box(win);
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
    menu->add_item("{R}Quit (Don't Save)");

    int result = menu->render();
    switch(result)
    {
        case 0: about(); break;
        case 1: quit_no_save(); break;
    }
}

// Quits the game without saving.
void SystemMenu::quit_no_save()
{
    if (!UI::are_you_sure()) return;
    core()->cleanup();
    exit(EXIT_SUCCESS);
}

}   // namespace invictus
