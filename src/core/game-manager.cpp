// core/game-manager.cpp -- The GameManager class manages the currently-running game state, as well as handling save/load functions.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/gen-dungeon.hpp"
#include "area/tile.hpp"
#include "codex/codex-item.hpp"
#include "codex/codex-tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/save-load.hpp"
#include "entity/item.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "tune/timing.hpp"
#include "ui/system-menu.hpp"
#include "ui/title.hpp"
#include "ui/ui.hpp"
#include "util/filex.hpp"
#include "util/strx.hpp"


namespace invictus
{

// The skull symbol to render on the game-over screen.
uint8_t GameManager::skull_pattern[4] = { 0x70, 0xFA, 0xED, 0xFB };


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

// The player has just died.
void GameManager::die()
{
    if (game_state_ == GameState::DUNGEON_DEAD || game_state_ == GameState::GAME_OVER) return;
    erase_save_files();
    core()->message("{m}You have died without honour! {r}Press the space bar to continue...");
    game_state_ = GameState::DUNGEON_DEAD;
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

// Deletes the save files in the current save folder.
void GameManager::erase_save_files() { FileX::delete_files_in_dir(save_folder_); }

// Brøther, may I have some lööps?
void GameManager::game_loop()
{
    auto terminal = core()->terminal();
    auto guru = core()->guru();

    // The entry game state should be TITLE by this point.
    if (game_state_ == GameState::TITLE)
    {
        auto title = std::make_unique<TitleScreen>();
        title->title_screen();
    }
    else core()->guru()->halt("Unknown entry game state", static_cast<int>(game_state_));

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
    else core()->guru()->halt("Unknown entry game state", static_cast<int>(game_state_));

    core()->guru()->log("Starting main game lööp, brøther.");
    int key = 0;
    while (true)
    {
        switch(game_state_)
        {
            case GameState::DUNGEON: dungeon_input(key); break;
            case GameState::DUNGEON_DEAD: if (key == ' ') game_over_screen(GameOverType::DEAD); break;
            default:
                guru->halt("Invalid game state!", static_cast<int>(game_state_));
                break;
        }

        tick();
        if (player_->is_dead()) die();
        ui_->render();

        key = terminal->get_key();
        if (key == Key::RESIZE) ui_->window_resized();
    }
}

// Renders the game-over screen.
void GameManager::game_over_screen(GameOverType type)
{
    auto terminal = core()->terminal();
    ui_->dungeon_mode_ui(false);
    game_state_ = GameState::GAME_OVER;
    bool redraw = true;
    int key = 0;

    while(true)
    {
        const int midcol = terminal->get_midcol(), midrow = terminal->get_midrow();

        if (redraw)
        {
            redraw = false;
            terminal->cls();

            for (int x = 0; x < 7; x++)
            {
                const int ax = midcol - 7 + (x * 2);
                uint8_t line_code = skull_pattern[x < 4 ? x : 6 - x ];
                for (int y = 0; y < 8; y++)
                {
                    const int ay = midrow - 5 + y;
                    if (line_code & (1 << (7 - y)))
                    {
                        terminal->put(' ', ax, ay, Colour::RED, PRINT_FLAG_REVERSE);
                        terminal->put(' ', ax + 1, ay, Colour::RED, PRINT_FLAG_REVERSE);
                    }
                }
            }
            std::string top_line;
            switch(type)
            {
                case GameOverType::DEAD: top_line = "YOU HAVE DIED... YOUR ADVENTURE HAS COME TO AN END"; break;
                case GameOverType::FAILED: top_line = "YOU HAVE ESCAPED WITH YOUR LIFE... BUT WITH NO GLORY"; break;
                case GameOverType::SUCCESS: top_line = "YOU HAVE BROUGHT HONOUR AND GLORY TO THE KINGDOM"; break;
            }
            terminal->print(top_line, midcol - (top_line.size() / 2), midrow - 8, Colour::RED_BOLD);
            terminal->print("{g}.~{r}* {R}THANKS FOR PLAYING MORIOR INVICTUS {r}*{g}~.", midcol - 21, midrow + 5);
            terminal->print("PRESS THE SPACE BAR WHEN YOU ARE READY TO MOVE ON", midcol - 25, midrow + 7, Colour::RED_BOLD);
            terminal->flip();
        }

        key = terminal->get_key();
        if (key == Key::RESIZE) redraw = true;
        else if (key == ' ')
        {
            core()->cleanup();
            exit(EXIT_SUCCESS);
        }
    }
}

// Retrieves the current state of the game.
GameState GameManager::game_state() const { return game_state_; }

// Sets up for a new game.
void GameManager::new_game()
{
    auto terminal = core()->terminal();
    std::string intro_str = "Welcome to the prototype {W}0.1 {w}build of {R}Morior Invictus{w}! You must delve deep into {Y}the Tomb of Forgotten Kings {w}and \
seek out the long-lost {M}Crown of Kings{w}, then return to the surface with your prize!";
    bool redraw = true;
    while (true)
    {
        if (redraw)
        {
            std::vector<std::string> intro_vec = StrX::string_explode_colour(intro_str, terminal->get_cols());
            terminal->cls();
            for (unsigned int i = 0; i < intro_vec.size(); i++)
                terminal->print(intro_vec.at(i), 0, i);
            int line = intro_vec.size() + 1;
            terminal->print("{C}GAME CONTOLS:", 0, line++);
            terminal->print("{G}Arrow Keys{w}, {G}Numeric Keypad{w} or {G}vi keys {g}(h j k l y u b n) {w}- movement", 0, line++);
            terminal->print("{G}< {w}or {G}> {w}- go up or down stairs", 0, line++);
            terminal->print("{G}. {w}or {G}g {w}- interact with items on the ground", 0, line++);
            terminal->print("{G}e {w}- check equipment", 0, line++);
            terminal->print("{G}i {w}- check carried items", 0, line++);
            terminal->print("{G}o {w}- open a door", 0, line++);
            terminal->print("{G}c {w}- close a door", 0, line++);
            terminal->print("{G}shift-S {w}- save the game", 0, line++);
            terminal->print("{G}, {w}or {G}numpad 5 {w}- wait for a moment", 0, line++);
            terminal->print("{G}= {w}- open main menu", 0, line++);
            terminal->print("{G}space bar {w}- close a menu", 0, line++);
            line++;
            terminal->print("{C}To load a saved game, specify the -load parameter on the command-line.", 0, line++);
            line++;
            terminal->print("{R}PRESS THE SPACE BAR TO BEGIN", 0, line++);
            line++;
            terminal->print("{R}IF YOU DO NOT WISH TO LOSE AN EXISTING SAVE FILE, CLOSE THE GAME (CTRL-C) NOW!", 0, line++);
            terminal->flip();
            redraw = false;
        }
        int key = terminal->get_key();
        if (key == Key::RESIZE)
        {
            redraw = true;
            core()->game()->ui()->window_resized();
        }
        else if (key == ' ') break;
    }
    terminal->cls();

    erase_save_files();
    area_ = std::make_shared<Area>(50, 50);
    area_->set_level(1);
    area_->set_file("tfk");
    auto generator = std::make_unique<DungeonGenerator>(area_);
    generator->generate();
    auto stair_coords = area_->find_tile_tag(TileTag::StairsUp);
    player_->set_pos(stair_coords.first, stair_coords.second);
    player_->set_equipment(EquipSlot::BODY, ItemID::LEATHER_ARMOUR);
    player_->set_equipment(EquipSlot::HAND_MAIN, ItemID::LONGSWORD);
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

    bool has_crown_of_kings = false;
    if (player_->equipment(EquipSlot::HEAD)->name() == "{M}The Crown of Kings") has_crown_of_kings = true;
    else
    {
        for (auto item : *player_->inv())
        {
            if (item->name() == "{M}The Crown of Kings")
            {
                has_crown_of_kings = true;
                break;
            }
        }
    }

    if (new_level <= 0 && !has_crown_of_kings)
    {
        core()->message("{y}Are you sure you want to return to the surface?");
        if (!UI::are_you_sure())
        {
            core()->message("{u}Wisely, you decide to remain.");
            return;
        }
    }

    std::string current_area_string = area_->file_str();
    area_->set_player_left(player_->x(), player_->y());
    SaveLoad::save_area_to_file(save_folder_ + "/" + area_->filename() + ".dat", area_);
    std::string travel_string;
    if (up) travel_string = "{c}You ascend the stairs to the previous level...";
    else travel_string = "{c}You descend the stairs to the next level...";
    core()->message(travel_string);

    if (new_level <= 0)
    {
        erase_save_files();
        if (has_crown_of_kings) game_over_screen(GameOverType::SUCCESS);
        else game_over_screen(GameOverType::FAILED);
    }

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

        // If this is the lowest level, place the Crown of Kings where the down staircase would be.
        if (new_level == 5)
        {
            auto down_stairs = area_->find_tile_tag(TileTag::StairsDown);
            area_->set_tile(down_stairs.first, down_stairs.second, TileID::FLOOR_STONE);
            auto crown = CodexItem::generate(ItemID::CROWN_OF_KINGS);
            crown->set_pos(down_stairs.first, down_stairs.second);
            area_->entities()->push_back(crown);
        }
    }
    core()->game()->ui()->full_redraw();
    SaveLoad::save_game();
}

}   // namespace invictus
