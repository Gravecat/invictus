// core/game-manager.cpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "ui/ui.hpp"

#include "factory/factory-tile.hpp" // temp


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

    if (game_state_ == GameState::NEW_GAME)
    {
        core()->guru()->log("Setting up new game...");
        new_game();
        game_state_ = GameState::DUNGEON;
    }

    core()->guru()->log("Starting main game lööp, brøther.");
    while (game_state_ != GameState::QUIT)
    {
        ui_->render();

        int key = terminal->get_key();
        if (key == Key::CLOSE) break;
        else if (key == Key::RESIZE) ui_->window_resized();
    }
}

// Retrieves the current state of the game.
GameState GameManager::game_state() const { return game_state_; }

// Sets up for a new game.
void GameManager::new_game()
{
    area_ = std::make_shared<Area>(30, 30);
    for (int x = 0; x < 30; x++)
    {
        for (int y = 0; y < 30; y++)
        {
            if (x == 0 || x == 29) area_->set_tile(x, y, TileID::WALL_BEDROCK);
            else if (y == 0 || y == 29) area_->set_tile(x, y, TileID::WALL_BEDROCK);
            else area_->set_tile(x, y, TileID::FLOOR_STONE);
        }
    }
    player_->set_pos(5, 5);
}

// Returns a pointer to the player character object.
const std::shared_ptr<Player> GameManager::player() const { return player_; }

// Sets the game state.
void GameManager::set_game_state(GameState new_state) { game_state_ = new_state; }

// Returns a pointer to the user interface manager.
const std::shared_ptr<UI> GameManager::ui() const { return ui_; }

}   // namespace invictus
