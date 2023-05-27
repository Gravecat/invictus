// core/core.hpp -- Main program entry, initialization and cleanup routines, along with links to the key subsystems of the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <exception>
#include <iostream>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/prefs.hpp"
#include "dev/acs-display.hpp"
#include "dev/keycode-check.hpp"
#include "terminal/terminal.hpp"
#include "ui/msglog.hpp"
#include "ui/ui.hpp"
#include "util/filex.hpp"
#include "util/winx.hpp"


namespace invictus { std::shared_ptr<Core> invictus_core = nullptr; }   // The main Core object.

// Main program entry point. Must be OUTSIDE the invictus namespace.
int main(int argc, char** argv)
{
#ifdef INVICTUS_TARGET_WINDOWS
    // Check if invictus.exe is already running (Windows only).
    invictus::WinX::check_if_already_running();
#endif

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

    try
    {
        // Check for command-line options.
        bool normal_start = true;
        if (parameters.size() >= 2)
        {
            for (auto param : parameters)
            {
                if (!param.compare("-keycode-check"))
                {
                    invictus::DevKeycodeCheck::begin();
                    normal_start = false;
                }
                if (!param.compare("-acs-display"))
                {
                    invictus::DevACSDisplay::display_test();
                    normal_start = false;
                }
                if (!param.compare("-load"))    // very much temporary, for testing/developing the save/load code
                {
                    invictus::core()->game()->set_game_state(invictus::GameState::LOAD_GAME);
                    invictus::core()->game()->game_loop();
                    normal_start = false;
                }
            }
        }
        parameters.clear();

        // Start the main game loop, unless we're running an abnormal start.
        if (normal_start)
        {
            invictus::core()->game()->set_game_state(invictus::GameState::NEW_GAME);
            invictus::core()->game()->game_loop();
        }
    }
    catch (std::exception &e) { invictus::core()->guru()->halt(e); }

    // Trigger cleanup code.
    invictus::core()->cleanup();
    return EXIT_SUCCESS;
}

namespace invictus
{

// Constructor, sets some default values.
Core::Core() : cleanup_done_(false), game_manager_(nullptr), guru_meditation_(nullptr), prefs_(nullptr), terminal_(nullptr) { }

// Destructor, calls cleanup code.
Core::~Core() { cleanup(); }

// Attempts to gracefully clean up memory and subsystems.
void Core::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    if (guru_meditation_) guru_meditation_->log("Attempting to shut down cleanly.");
    if (game_manager_)  // Clean up the high-level game state.
    {
        game_manager_->cleanup();
        game_manager_ = nullptr;
    }
    if (guru_meditation_)   // Clean up the Guru Meditation system and shut down.
    {
        guru_meditation_->cleanup();
        guru_meditation_ = nullptr;
    }
    if (terminal_)  // Run cleanup code on Terminal.
    {
        terminal_->cleanup();
        terminal_ = nullptr;
    }
    prefs_ = nullptr;   // There should be no destructor/cleanup code to worry about here.
}

// Returns a pointer to the GameManager object.
const std::shared_ptr<GameManager> Core::game() const { return game_manager_; }

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

    // Load user preferences.
    prefs_ = std::make_shared<Prefs>("userdata/prefs.txt");
    prefs_->load();
    prefs_->save();

    // Sets up the terminal emulator (Curses)
    terminal_ = std::make_shared<Terminal>();

    // Set up the game manager.
    game_manager_ = std::make_shared<GameManager>();
}

// A shortcut to core()->game()->ui()->msglog()->message().
void Core::message(std::string msg, unsigned char awaken_chance)
{
    if (!game_manager_ || !game_manager_->ui() || !game_manager_->ui()->msglog())
    {
        guru_meditation_->nonfatal("Attempt to send message to log before the GUI has been properly set up!", GURU_WARN);
        guru_meditation_->log("The message: " + msg, GURU_INFO);
        return;
    }
    game_manager_->ui()->msglog()->message(msg, awaken_chance);
}

// Returns a pointer to the user preferences object.
const std::shared_ptr<Prefs> Core::prefs() const { return prefs_; }

// Returns a pointer  to the terminal emulator object.
const std::shared_ptr<Terminal> Core::terminal() const { return terminal_; }

// Allows external access to the main Core object.
const std::shared_ptr<Core> core()
{
    if (!invictus_core) exit(EXIT_FAILURE);
    else return invictus_core;
}

}   // namespace invictus
