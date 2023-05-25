// core/game-manager.hpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_GAME_MANAGER_HPP_
#define CORE_GAME_MANAGER_HPP_

#include <memory>


namespace invictus
{

class Area;     // defined in area/area.hpp
class Player;   // defined in entity/player.hpp
class UI;       // defined in ui/ui.hpp


enum class GameState : uint8_t { INITIALIZING, QUIT, NEW_GAME, DUNGEON };

class GameManager
{
public:
                GameManager();      // Constructor, sets default values.
                ~GameManager();     // Destructor, calls cleanup code.
    void        cleanup();          // Cleans up anything that needs cleaning up.
    void        game_loop();        // Brøther, may I have some lööps?
    GameState   game_state() const; // Retrieves the current state of the game.
    void        set_game_state(GameState new_state);    // Sets the game state.

    const std::shared_ptr<Area>     area() const;   // Returns a pointer to the currently-loaded Area, if any.
    const std::shared_ptr<Player>   player() const; // Returns a pointer to the player character object.
    const std::shared_ptr<UI>       ui() const;     // Returns a pointer to the user interface manager.

private:
    void    new_game(); // Sets up for a new game.

    std::shared_ptr<Area>   area_;  // The currently-loaded Area of the game world.
    bool        cleanup_done_;      // Has the cleanup routine already run once?
    GameState   game_state_;        // The current game state.
    std::shared_ptr<Player> player_;    // The player character object.
    std::shared_ptr<UI> ui_;        // The user interface manager.
};

}       // namespace invictus
#endif  // CORE_GAME_MANAGER_HPP_
