// dev/console.hpp -- Debug/cheat console, where the player can enter various commands.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef DEV_CONSOLE_HPP_
#define DEV_CONSOLE_HPP_

#include <string>


namespace invictus
{

class DevConsole
{
public:
    static void open_dev_console(); // Opens the dev console and accepts commands from the player.

private:
    static void process_command(std::string cmd);   // Processes a dev console command.
};

}       // namespace invictus
#endif  // DEV_CONSOLE_HPP_
