// core/game-manager.hpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_GAME_MANAGER_HPP_
#define CORE_GAME_MANAGER_HPP_

namespace invictus
{

enum class GameState : uint8_t { INITIALIZING, QUIT };

class GameManager
{
public:
                GameManager();      // Constructor, sets default values.
                ~GameManager();     // Destructor, calls cleanup code.
    void        cleanup();          // Cleans up anything that needs cleaning up.
    void        game_loop();        // Brøther, may I have some lööps?
    GameState   game_state() const; // Retrieves the current state of the game.
    void        set_game_state(GameState new_state);    // Sets the game state.

private:
    bool        cleanup_done_;  // Has the cleanup routine already run once?
    GameState   game_state_;    // The current game state.
};

}       // namespace invictus
#endif  // CORE_GAME_MANAGER_HPP_
