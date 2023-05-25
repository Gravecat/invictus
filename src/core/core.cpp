// core/core.hpp -- Main program entry, initialization and cleanup routines, along with links to the key subsystems of the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <exception>
#include <iostream>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "terminal/terminal.hpp"
#include "util/filex.hpp"


namespace invictus { std::shared_ptr<Core> invictus_core = nullptr; }   // The main Core object.

// Main program entry point. Must be OUTSIDE the invictus namespace.
int main(int argc, char** argv)
{
    // Check command-line parameters.
    std::vector<std::string> parameters(argv, argv + argc);

    // Create the main Core object.
    try
    {
        invictus::invictus_core = std::make_shared<invictus::Core>();
        invictus::invictus_core->init(parameters);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (!invictus::invictus_core) return EXIT_FAILURE;  // This should never happen, but just in case...

    // Game manager setup and main game loop goes here.
    invictus::core()->guru()->halt("Test error!", 61050, 10248);

    invictus::invictus_core = nullptr;  // Trigger destructor cleanup code.
    return EXIT_SUCCESS;
}

namespace invictus
{

// Constructor, sets some default values.
Core::Core() : guru_meditation_(nullptr), terminal_(nullptr) { }

// Destructor, cleans up memory used and subsystems.
Core::~Core()
{
    cleanup();
}

// Attempts to gracefully clean up memory and subsystems.
void Core::cleanup()
{
    if (guru_meditation_)
    {
        guru_meditation_->log("Attempting to shut down cleanly.");
        guru_meditation_->log("Cleaning up Curses terminal.");
    }
    terminal_ = nullptr;        // Run destructor cleanup code on Terminal.
    guru_meditation_ = nullptr; // The Guru system goes last.
}

// Returns a pointer to the Guru Meditation object.
const std::shared_ptr<Guru> Core::guru() const
{
    if (!guru_meditation_) exit(EXIT_FAILURE);
    return guru_meditation_;
}

// Sets up the core game classes and data, and the terminal subsystem.
void Core::init(std::vector<std::string>)
{
    // Create user data folders.
    FileX::make_dir("userdata");
    FileX::make_dir("userdata/save");

    // Sets up the error-handling subsystem.
    guru_meditation_ = std::make_shared<Guru>("userdata/log.txt");

    // Sets up the terminal emulator (Curses)
    terminal_ = std::make_shared<Terminal>();
}

// Returns a pointer  to the terminal emulator object.
const std::shared_ptr<Terminal> Core::terminal() const { return terminal_; }

// Allows external access to the main Core object.
const std::shared_ptr<Core> core()
{
    if (!invictus_core) exit(EXIT_FAILURE);
    else return invictus_core;
}

}   // namespace invictus
