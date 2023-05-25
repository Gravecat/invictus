// ui/ui.cpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/msglog.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor, sets up UI elements.
UI::UI() : cleanup_done_(false), dungeon_needs_redraw_(true), dungeon_view_(nullptr), message_log_(std::make_shared<MessageLog>()),
    message_log_needs_redraw_(true), message_log_window_(nullptr)
{
    generate_dungeon_view();
    generate_message_log();
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
    if (dungeon_view_) dungeon_view_ = nullptr;
    if (message_log_window_) message_log_window_ = nullptr;
    if (message_log_) message_log_ = nullptr;
}

// Gets a pointer to the dungeon view window.
const std::shared_ptr<Window> UI::dungeon_view() const { return dungeon_view_; }

// Generates the dungeon view window.
void UI::generate_dungeon_view()
{
    auto terminal = core()->terminal();
    dungeon_view_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2, 0, 0);
}

// Generates the message log window.
void UI::generate_message_log()
{
    auto terminal = core()->terminal();
    message_log_window_ = std::make_shared<Window>(terminal->get_cols() - NEARBY_BAR_WIDTH, MESSAGE_LOG_HEIGHT, 0, terminal->get_rows() - MESSAGE_LOG_HEIGHT);
}

// Gets a pointer to the message log window.
const std::shared_ptr<Window> UI::message_log_window() const { return message_log_window_; }

// Gets a pointer to the message log object.
const std::shared_ptr<MessageLog> UI::msglog() const { return message_log_; }

// Marks the dungeon view as needing to be redrawn.
void UI::redraw_dungeon() { dungeon_needs_redraw_ = true; }

// Marks the message log window as needing to be redrawn.
void UI::redraw_message_log() { message_log_needs_redraw_ = true; }

// Renders the UI elements, if needed.
void UI::render()
{
    if (dungeon_needs_redraw_)
    {
        core()->terminal()->cls(dungeon_view_);
        core()->game()->area()->render();
        dungeon_needs_redraw_ = false;
    }
    if (message_log_needs_redraw_)
    {
        core()->terminal()->cls(message_log_window_);
        message_log_->render();
        message_log_needs_redraw_ = false;
    }
    core()->terminal()->flip();
}

// The terminal window has been resized.
void UI::window_resized()
{
    generate_dungeon_view();
    generate_message_log();
    dungeon_needs_redraw_ = true;
    message_log_needs_redraw_ = true;
}

}   // namespace invictus
