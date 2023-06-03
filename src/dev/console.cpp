// dev/console.cpp -- Debug/cheat console, where the player can enter various commands.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <vector>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "dev/console.hpp"
#include "ui/msglog.hpp"
#include "ui/ui.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Opens the dev console and accepts commands from the player.
void DevConsole::open_dev_console()
{
    auto msglog = core()->game()->ui()->msglog();
    std::string input;
    while(true)
    {
        input = msglog->get_string();
        if (!input.size())
        {
            core()->message("{C}Console closed.");
            return;
        }
        else process_command(input);
    }
}

// Processes a dev console command.
void DevConsole::process_command(std::string cmd)
{
    std::vector<std::string> words = StrX::string_explode(cmd, " ");

    if (words.at(0) == "qns")
    {
        core()->cleanup();
        exit(EXIT_SUCCESS);
    }
    else core()->message("{y}Unknown console command: " + words.at(0));
}

}   // namespace invictus
