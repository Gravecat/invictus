// core/game-manager.hpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_GAME_MANAGER_HPP_
#define CORE_GAME_MANAGER_HPP_

#include <memory>
#include <string>


namespace invictus
{

class Area;     // defined in area/area.hpp
class Player;   // defined in entity/player.hpp
class UI;       // defined in ui/ui.hpp


enum class GameState : uint8_t { INITIALIZING, NEW_GAME, LOAD_GAME, DUNGEON, DUNGEON_DEAD, GAME_OVER };

enum class GameOverType : uint8_t { DEAD, FAILED, SUCCESS };


class GameManager
{
public:
                GameManager();      // Constructor, sets default values.
                ~GameManager();     // Destructor, calls cleanup code.
    void        cleanup();          // Cleans up anything that needs cleaning up.
    void        erase_save_files(); // Deletes the save files in the current save folder.
    void        die();              // The player has just died.
    void        game_loop();        // Brøther, may I have some lööps?
    GameState   game_state() const; // Retrieves the current state of the game.
    void        pass_time(float time);  // The player has taken an action which causes some time to pass.
    const std::string save_folder() const;  // Retrieves the name of the saved game folder currently in use.
    void        set_game_state(GameState new_state);    // Sets the game state.
    void        tick();             // Processes non-player actions and progresses the world state.

    const std::shared_ptr<Area>     area() const;   // Returns a pointer to the currently-loaded Area, if any.
    const std::shared_ptr<Player>   player() const; // Returns a pointer to the player character object.
    const std::shared_ptr<UI>       ui() const;     // Returns a pointer to the user interface manager.

private:
    void    dungeon_input(int key);     // Handles the player's input, when in dungeon mode.
    void    game_over_screen(GameOverType type);        // Renders the game-over screen.
    void    new_game();                 // Sets up for a new game.
    void    use_stairs(bool up);        // Attempts to go up or down stairs.

    std::shared_ptr<Area>   area_;  // The currently-loaded Area of the game world.
    bool        cleanup_done_;      // Has the cleanup routine already run once?
    GameState   game_state_;        // The current game state.
    float       heartbeat_;         // The main timer of the world, incremented when the player takes actions.
    float       heartbeat10_;       // As above, but this one's a slower heartbeat that causes things like buffs/debuffs to trigger at a 1/10 speed rate
    std::shared_ptr<Player> player_;    // The player character object.
    std::string save_folder_;       // The saved game folder currently in use.
    std::shared_ptr<UI> ui_;        // The user interface manager.

    static uint8_t skull_pattern[4];    // The skull symbol to render on the game-over screen.

friend class SaveLoad;
};

}       // namespace invictus
#endif  // CORE_GAME_MANAGER_HPP_
