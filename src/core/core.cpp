// core/core.hpp -- Main program entry, initialization and cleanup routines, along with links to the key subsystems of the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>

#include "core/core.hpp"
#include "core/terminal.hpp"


std::shared_ptr<Core> invictus_core = nullptr;  // The main Core object.

// Main program entry point.
int main(int argc, char** argv)
{
    // Check command-line parameters.
    std::vector<std::string> parameters(argv, argv + argc);

    // Create the main Core object.
    try
    {
        invictus_core = std::make_shared<Core>(parameters);
    }
    catch (std::exception &e)
    {
        //core()->guru()->halt(e.what());
        return EXIT_FAILURE;
    }
    if (!invictus_core) return EXIT_FAILURE;    // This should never happen, but just in case...

    // Game manager setup and main game loop goes here.

    invictus_core = nullptr;    // Trigger destructor cleanup code.
    return EXIT_SUCCESS;
}

// Sets up the core game classes and data, and the terminal subsystem.
Core::Core(std::vector<std::string>) : terminal_(nullptr)
{
    // Sets up the terminal emulator (Curses)
    terminal_ = std::make_shared<Terminal>();
}

// Destructor, cleans up memory used and subsystems.
Core::~Core()
{
    terminal_ = nullptr;    // Run destructor cleanup code on Terminal.
}

// Returns a pointer  to the terminal emulator object.
const std::shared_ptr<Terminal> Core::terminal() const { return terminal_; }

// Allows external access to the main Core object.
const std::shared_ptr<Core> core()
{
    if (!invictus_core) exit(EXIT_FAILURE);
    else return invictus_core;
}
