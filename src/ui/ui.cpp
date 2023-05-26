// ui/ui.cpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/bars.hpp"
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

// Generates the dungeon view window.
void UI::generate_dungeon_view()
{
    auto terminal = core()->terminal();
    dungeon_view_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2, 0, 0);
    if (core()->game() && core()->game()->game_state() == GameState::DUNGEON) dungeon_view_->set_visible(true);
    else dungeon_view_->set_visible(false);
}

// Generates the message log window.
void UI::generate_message_log()
{
    auto terminal = core()->terminal();
    message_log_window_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH + 1, MESSAGE_LOG_HEIGHT, 0, terminal->get_rows() - MESSAGE_LOG_HEIGHT);
    if (core()->game() && core()->game()->game_state() == GameState::DUNGEON) message_log_window_->set_visible(true);
    else message_log_window_->set_visible(false);
}

// Generates the nearby sidebar window.
void UI::generate_nearby_window()
{
    auto terminal = core()->terminal();
    nearby_window_ = std::make_shared<Window>(NEARBY_BAR_WIDTH, terminal->get_rows(), terminal->get_cols() - NEARBY_BAR_WIDTH, 0);
    if (core()->game() && core()->game()->game_state() == GameState::DUNGEON) nearby_window_->set_visible(true);
    else nearby_window_->set_visible(false);
}

// Generates the stat bars window.
void UI::generate_stat_bars()
{
    auto terminal = core()->terminal();
    stat_bars_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, 2, 0, terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2);
    if (core()->game() && core()->game()->game_state() == GameState::DUNGEON) stat_bars_->set_visible(true);
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
void UI::render()
{
    bool flip = false;
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
        Bars::render_health_mana_bars();
        stat_bars_need_redraw_ = false;
        flip = true;
    }
    if (flip) core()->terminal()->flip();
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
    dungeon_needs_redraw_ = message_log_needs_redraw_ = nearby_needs_redraw_ = stat_bars_need_redraw_ = true;
}

}   // namespace invictus
