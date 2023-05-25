// core/game-manager.cpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor, sets default values.
GameManager::GameManager() : area_(nullptr), cleanup_done_(false), game_state_(GameState::INITIALIZING), player_(std::make_shared<Player>()),
    ui_(std::make_shared<UI>())
{ core()->guru()->log("Game manager ready!"); }

// Destructor, calls cleanup code.
GameManager::~GameManager() { cleanup(); }

// Returns a pointer to the currently-loaded Area, if any.
const std::shared_ptr<Area> GameManager::area() const { return area_; }

// Cleans up anything that needs cleaning up.
void GameManager::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    if (core()->guru()) core()->guru()->log("Cleaning up the game state.");
    if (ui_)
    {
        ui_->cleanup();
        ui_ = nullptr;
    }
    if (area_)
    {
        area_->cleanup();
        area_ = nullptr;
    }
    if (player_) player_ = nullptr;
}

// Brøther, may I have some lööps?
void GameManager::game_loop()
{
    auto terminal = core()->terminal();
    int key = 0;
    bool do_redraw = true;

    core()->guru()->log("Starting main game lööp, brøther.");
    while (game_state_ != GameState::QUIT)
    {
        key = terminal->get_key();
        if (key == Key::CLOSE) break;
        else if (key == Key::RESIZE) do_redraw = true;

        if (do_redraw)
        {
            terminal->flip();
            do_redraw = false;
        }
    }
}

// Retrieves the current state of the game.
GameState GameManager::game_state() const { return game_state_; }

// Returns a pointer to the player character object.
const std::shared_ptr<Player> GameManager::player() const { return player_; }

// Sets the game state.
void GameManager::set_game_state(GameState new_state) { game_state_ = new_state; }

// Returns a pointer to the user interface manager.
const std::shared_ptr<UI> GameManager::ui() const { return ui_; }

}   // namespace invictus
