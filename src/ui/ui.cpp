// ui/ui.cpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/buff.hpp"
#include "entity/player.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "tune/message-log.hpp"
#include "tune/nearby-bar.hpp"
#include "ui/bars.hpp"
#include "ui/menu.hpp"
#include "ui/msglog.hpp"
#include "ui/nearby.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor, sets up UI elements.
UI::UI() : cleanup_done_(false), dungeon_needs_redraw_(true), dungeon_view_(nullptr), message_log_(std::make_shared<MessageLog>()),
    message_log_needs_redraw_(true), message_log_window_(nullptr), nearby_needs_redraw_(true), nearby_window_(nullptr), stat_bars_(nullptr),
    stat_bars_need_redraw_(true)
{
    window_resized();   // (re)create the UI windows.
    core()->guru()->log("User interface manager ready!");
}

// Destructor, calls cleanup function.
UI::~UI() { cleanup(); }

// Renders an "are you sure?" window.
bool UI::are_you_sure()
{
    auto menu = std::make_unique<Menu>();
    menu->set_title("Are you sure?");
    menu->add_item("{R}No...");
    menu->add_item("{G}Yes!!");
    int result = menu->render();
    if (result == 1) return true;
    else return false;
}

// Cleans up any sub-elements.
void UI::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    core()->guru()->log("Cleaning up user interface.");
    dungeon_view_ = nullptr;
    message_log_window_ = nullptr;
    message_log_ = nullptr;
    nearby_window_ = nullptr;
    stat_bars_ = nullptr;
}

// Enables or disables the dungeon-mode UI.
void UI::dungeon_mode_ui(bool enable)
{
    if (dungeon_view_) dungeon_view_->set_visible(enable);
    if (message_log_window_) message_log_window_->set_visible(enable);
    if (nearby_window_) nearby_window_->set_visible(enable);
    if (stat_bars_) stat_bars_->set_visible(enable);
}

// Gets a pointer to the dungeon view window.
const std::shared_ptr<Window> UI::dungeon_view() const { return dungeon_view_; }

// Everything wants redrawing.
void UI::full_redraw() { dungeon_needs_redraw_ = message_log_needs_redraw_ = nearby_needs_redraw_ = stat_bars_need_redraw_ = true; }

// Generates the dungeon view window.
void UI::generate_dungeon_view()
{
    auto terminal = core()->terminal();
    GameState game_state = (core()->game() ? core()->game()->game_state() : GameState::INITIALIZING);
    dungeon_view_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2, 0, 0);
    if (game_state == GameState::DUNGEON || game_state == GameState::DUNGEON_DEAD) dungeon_view_->set_visible(true);
    else dungeon_view_->set_visible(false);
}

// Generates the message log window.
void UI::generate_message_log()
{
    auto terminal = core()->terminal();
    GameState game_state = (core()->game() ? core()->game()->game_state() : GameState::INITIALIZING);
    message_log_window_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH + 1, MESSAGE_LOG_HEIGHT, 0, terminal->get_rows() -
        MESSAGE_LOG_HEIGHT);
    if (game_state == GameState::DUNGEON || game_state == GameState::DUNGEON_DEAD) message_log_window_->set_visible(true);
    else message_log_window_->set_visible(false);
}

// Generates the nearby sidebar window.
void UI::generate_nearby_window()
{
    auto terminal = core()->terminal();
    GameState game_state = (core()->game() ? core()->game()->game_state() : GameState::INITIALIZING);
    nearby_window_ = std::make_shared<Window>(NEARBY_BAR_WIDTH, terminal->get_rows(), terminal->get_cols() - NEARBY_BAR_WIDTH, 0);
    if (game_state == GameState::DUNGEON || game_state == GameState::DUNGEON_DEAD) nearby_window_->set_visible(true);
    else nearby_window_->set_visible(false);
}

// Generates the stat bars window.
void UI::generate_stat_bars()
{
    auto terminal = core()->terminal();
    GameState game_state = (core()->game() ? core()->game()->game_state() : GameState::INITIALIZING);
    stat_bars_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, 2, 0, terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2);
    if (game_state == GameState::DUNGEON || game_state == GameState::DUNGEON_DEAD) stat_bars_->set_visible(true);
    else stat_bars_->set_visible(false);
}

// Gets a pointer to the message log window.
const std::shared_ptr<Window> UI::message_log_window() const { return message_log_window_; }

// Gets a pointer to the message log object.
const std::shared_ptr<MessageLog> UI::msglog() const { return message_log_; }

// Gets a pointer to the nearby sidebar window.
const std::shared_ptr<Window> UI::nearby_window() const { return nearby_window_; }

// Marks the dungeon view as needing to be redrawn.
void UI::redraw_dungeon() { dungeon_needs_redraw_ = nearby_needs_redraw_ = true; }

// Marks the message log window as needing to be redrawn.
void UI::redraw_message_log() { message_log_needs_redraw_ = true; }

// Marks the nearby window as needing to be redrawn.
void UI::redraw_nearby() { nearby_needs_redraw_ = true; }

// Marks the stat bars as needing to be redrawn.
void UI::redraw_stat_bars() { stat_bars_need_redraw_ = true; }

// Renders the UI elements, if needed.
void UI::render(bool force_flip)
{
    bool flip = force_flip;
    if (dungeon_needs_redraw_)
    {
        core()->terminal()->cls(dungeon_view_);
        core()->game()->area()->render();
        dungeon_needs_redraw_ = false;
        flip = true;
    }
    if (message_log_needs_redraw_)
    {
        core()->terminal()->cls(message_log_window_);
        message_log_->render();
        message_log_needs_redraw_ = false;
        flip = true;
    }
    if (nearby_needs_redraw_)
    {
        core()->terminal()->cls(nearby_window_);
        Nearby::render();
        nearby_needs_redraw_ = false;
        flip = true;
    }
    if (stat_bars_need_redraw_)
    {
        core()->terminal()->cls(stat_bars_);
        render_stat_bars();
        stat_bars_need_redraw_ = false;
        flip = true;
    }
    if (flip) core()->terminal()->flip();
}

// Renders the player's health, mana and stamina bars.
void UI::render_stat_bars()
{
    auto terminal = core()->terminal();
    auto player = core()->game()->player();
    int window_w = stat_bars_->get_width();

    const int hp_bar_width = window_w / 2;
    const int sp_bar_width = (window_w - hp_bar_width) / 2;
    const int mp_bar_width = window_w - hp_bar_width - sp_bar_width;

    Bars::render_bar(0, 1, hp_bar_width, "HP", player->hp(), player->hp(true), Colour::RED_WHITE, BAR_FLAG_NUMBERS | BAR_FLAG_ROUND_UP, stat_bars_);
    Bars::render_bar(hp_bar_width, 1, sp_bar_width, "SP", player->sp(), player->sp(true), Colour::GREEN_WHITE, BAR_FLAG_NUMBERS, stat_bars_);
    Bars::render_bar(hp_bar_width + sp_bar_width, 1, mp_bar_width, "MP", player->mp(), player->mp(true), Colour::BLUE_WHITE, BAR_FLAG_NUMBERS, stat_bars_);

    std::string status_messages;
    if (player->has_buff(BuffType::PAIN)) status_messages += "{Y}[PAIN] ";

    if (status_messages.size()) terminal->print(status_messages.substr(0, status_messages.size() - 1), 0, 0, Colour::WHITE, 0, stat_bars_);
}

// Gets a pointer to the stat bars window.
const std::shared_ptr<Window> UI::stat_bars() const { return stat_bars_; }

// The terminal window has been resized.
void UI::window_resized()
{
    generate_dungeon_view();
    generate_message_log();
    generate_nearby_window();
    generate_stat_bars();
    if (message_log_ && core() && core()->game() && core()->game()->game_state() != GameState::INITIALIZING) message_log_->screen_resized();
    full_redraw();
}

}   // namespace invictus
