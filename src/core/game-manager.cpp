// core/game-manager.cpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/gen-dungeon.hpp"
#include "area/tile.hpp"
#include "codex/codex-item.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/save-load.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "tune/timing.hpp"
#include "ui/death.hpp"
#include "ui/system-menu.hpp"
#include "ui/ui.hpp"
#include "util/filex.hpp"


namespace invictus
{

// Constructor, sets default values.
GameManager::GameManager() : area_(nullptr), cleanup_done_(false), game_state_(GameState::INITIALIZING), heartbeat_(0), heartbeat10_(0),
    player_(std::make_shared<Player>()), save_folder_("userdata/save"), ui_(std::make_shared<UI>())
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

void GameManager::dungeon_input(int key)
{
    int dx = 0, dy = 0;
    switch(key)
    {
        case 0: return;

        case 'k': case Key::ARROW_UP: case Key::KP8: dy = -1; break;    // Move north
        case 'j': case Key::ARROW_DOWN: case Key::KP2: dy = 1; break;   // Move south
        case 'h': case Key::ARROW_LEFT: case Key::KP4: dx = -1; break;  // Move west
        case 'l': case Key::ARROW_RIGHT: case Key::KP6: dx = 1; break;  // Move east
        case 'b': case Key::KP1: dx = -1; dy = 1; break;    // Move southwest
        case 'n': case Key::KP3: dx = 1; dy = 1; break;     // Move southeast
        case 'y': case Key::KP7: dx = -1; dy = -1; break;   // Move northwest
        case 'u': case Key::KP9: dx = 1; dy = -1; break;    // Move northeast

        case ',': case Key::KP5: pass_time(TIME_DO_NOTHING); break; // Do nothing.
        case '.': player_->ground_items(); break;           // Interact with items on the ground.
        case '=': SystemMenu::open(); break;                // Opens the system menu.
        case '<': case '>': use_stairs(key == '<'); break;  // Goes up or down stairs.
        case 'c': player_->close_a_door(); break;           // Attempts to close something.
        case 'e': player_->take_inventory(true); break;     // Interact with equipped items.
        case 'g': player_->get_item(); break;               // Picks something up.
        case 'i': player_->take_inventory(); break;         // Interact with carried items.
        case 'o': player_->open_a_door(); break;            // Attempts to open something.
        case 'S': SaveLoad::save_game(); break;             // Saves the game!
    }

    if (dx || dy)
    {
        bool success = player_->move_or_attack(player_, dx, dy);
        if (success) ui_->redraw_dungeon();
    }
}

// Brøther, may I have some lööps?
void GameManager::game_loop()
{
    auto terminal = core()->terminal();
    auto guru = core()->guru();

    if (game_state_ == GameState::NEW_GAME)
    {
        core()->guru()->log("Setting up new game...");
        new_game();
    }
    else if (game_state_ == GameState::LOAD_GAME)
    {
        SaveLoad::load_game("userdata/save");
        ui_->window_resized();
    }

    core()->guru()->log("Starting main game lööp, brøther.");
    int key = 0;
    while (true)
    {
        switch(game_state_)
        {
            case GameState::DEAD: break;
            case GameState::DUNGEON: dungeon_input(key); break;
            case GameState::DUNGEON_DEAD: if (key == ' ') Death::render_death_screen(); break;
            case GameState::INITIALIZING: case GameState::NEW_GAME: case GameState::LOAD_GAME:
                guru->halt("Invalid game state!", static_cast<int>(game_state_));
                break;
        }

        tick();
        if (player_->is_dead()) Death::die();
        ui_->render();

        key = terminal->get_key();
        if (key == Key::RESIZE) ui_->window_resized();
    }
}

// Retrieves the current state of the game.
GameState GameManager::game_state() const { return game_state_; }

// Sets up for a new game.
void GameManager::new_game()
{
    FileX::delete_files_in_dir(save_folder_);
    area_ = std::make_shared<Area>(50, 50);
    area_->set_level(1);
    area_->set_file("tfk");
    auto generator = std::make_unique<DungeonGenerator>(area_);
    generator->generate();
    auto stair_coords = area_->find_tile_tag(TileTag::StairsUp);
    player_->set_pos(stair_coords.first, stair_coords.second);
    player_->inventory_add(CodexItem::generate(ItemID::LEATHER_ARMOUR));
    player_->inventory_add(CodexItem::generate(ItemID::SHORTSWORD));
    player_->inventory_add(CodexItem::generate(ItemID::LONGSWORD));
    player_->inventory_add(CodexItem::generate(ItemID::GREATSWORD));
    game_state_ = GameState::DUNGEON;
    ui_->dungeon_mode_ui(true);
}

// The player has taken an action which causes some time to pass.
void GameManager::pass_time(float time)
{
    heartbeat_ += time;
    heartbeat10_ += time;
}

// Returns a pointer to the player character object.
const std::shared_ptr<Player> GameManager::player() const { return player_; }

// Retrieves the name of the saved game folder currently in use.
const std::string GameManager::save_folder() const { return save_folder_; }

// Sets the game state.
void GameManager::set_game_state(GameState new_state) { game_state_ = new_state; }

// Processes non-player actions and progresses the world state.
void GameManager::tick()
{
    if (!area_) return;

    while (heartbeat_ >= TICK_SPEED)
    {
        heartbeat_ -= TICK_SPEED;
        for (auto entity : *area_->entities())
        {
            if (game_state_ != GameState::DUNGEON) break;   // If the game state changes, just stop processing Entity ticks.
            entity->tick(entity);
        }
    }

    while (heartbeat10_ >= TICK_SPEED * 10)
    {
        heartbeat10_ -= TICK_SPEED * 10;
        for (auto entity : *area_->entities())
        {
            if (game_state_ != GameState::DUNGEON) break;   // If the game state changes, just stop processing Entity ticks.
            entity->tick10(entity);
        }
    }
}

// Returns a pointer to the user interface manager.
const std::shared_ptr<UI> GameManager::ui() const { return ui_; }

// Attempts to go up or down stairs.
void GameManager::use_stairs(bool up)
{
    auto tile = area_->tile(player_->x(), player_->y());
    const bool stairs_up = tile->tag(TileTag::StairsUp), stairs_down = tile->tag(TileTag::StairsDown);
    if (up && !stairs_up)
    {
        if (stairs_down) core()->message("{y}You can only travel down from here.");
        else core()->message("{y}There is nowhere to ascend here.");
        return;
    }
    else if (!up && !stairs_down)
    {
        if (stairs_up) core()->message("{y}You can only travel up from here.");
        else core()->message("{y}There is nowhere to descend here.");
        return;
    }

    int current_level = area_->level();
    int new_level = current_level + (up ? -1 : 1);
    std::string current_area_string = area_->file_str();
    area_->set_player_left(player_->x(), player_->y());
    SaveLoad::save_game();
    std::string travel_string;
    if (up) travel_string = "{c}You ascend the stairs to the previous level...";
    else travel_string = "{c}You descend the stairs to the next level...";
    core()->message(travel_string);

    // Check if the new Area should be loaded from a file, or generated fresh.
    std::string filename = save_folder_ + "/" + current_area_string + std::to_string(new_level) + ".dat";
    core()->guru()->log(filename);
    if (FileX::file_exists(filename))
    {
        area_ = SaveLoad::load_area_from_file(filename);
        auto stair_coords = area_->get_player_left();
        player_->set_pos(stair_coords.first, stair_coords.second);
    }
    else
    {
        area_ = std::make_shared<Area>(50, 50);
        area_->set_level(new_level);
        area_->set_file(current_area_string);
        auto generator = std::make_unique<DungeonGenerator>(area_);
        generator->generate();
        auto stair_coords = area_->find_tile_tag(up ? TileTag::StairsDown : TileTag::StairsUp);
        player_->set_pos(stair_coords.first, stair_coords.second);
    }
    core()->game()->ui()->full_redraw();
}

}   // namespace invictus
