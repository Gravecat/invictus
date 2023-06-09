// ui/ui.cpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

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
#include "util/strx.hpp"


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
void UI::render(ForceFlipMode mode)
{
    bool flip = (mode == ForceFlipMode::FORCE_FLIP);
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
    if (flip && mode != ForceFlipMode::FORCE_NO_FLIP) core()->terminal()->flip();
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
    if (player->has_buff(BuffType::PAIN)) status_messages += "{Y}[PAIN]{b} ";
    if (status_messages.size()) terminal->print(status_messages.substr(0, status_messages.size() - 1), 0, 0, Colour::WHITE, PRINT_FLAG_REVERSE, stat_bars_);

    int level = core()->game()->area()->level();
    if (level > 0)
    {
        std::string level_str = std::to_string(level * 50) + "'";
        terminal->print(level_str, window_w - level_str.size(), 0, Colour::BLACK_BOLD, PRINT_FLAG_REVERSE, stat_bars_);
    }
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

// Renders a yes/no dialogue box. To fit in better with different window/UI layouts, it'll return Key::RESIZED if the screen is resized, so the underlying
// windows can be re-rendered, then it can be safely called again. If successful, it will return 'Y' (89) for a yes, or 'N' (78) for a no.
int UI::yes_no(const std::string &message, const std::string &title)
{
    std::vector<std::string> lines = StrX::string_explode_colour(message, 40);
    int widest = std::max(10, static_cast<int>(title.size() + 2));
    for (auto line : lines)
    {
        int len = StrX::strlen_colour(line);
        if (len > widest) widest = len;
    }
    auto terminal = core()->terminal();
    auto game = core()->game();
    auto yes_no_window = std::make_shared<Window>(widest + 4, 5 + (lines.size() ? 1 + lines.size() : 0));
    terminal->box(yes_no_window);
    
    yes_no_window->move(terminal->get_midcol() - yes_no_window->get_width() / 2, terminal->get_midrow() - yes_no_window->get_height() / 2);
    terminal->print(" " + title + " ", yes_no_window->get_width() / 2 - title.size() / 2 - 1, 0, Colour::WHITE, PRINT_FLAG_REVERSE, yes_no_window);
    for (unsigned int i = 0; i < lines.size(); i++)
        terminal->print(lines.at(i), 2, 2 + i, Colour::WHITE, 0, yes_no_window);

    int yes_no_y_pos = (lines.size() ? lines.size() + 3 : 2);
    int yes_no_x_pos = yes_no_window->get_width() / 2 - 5;
    bool result = false;
    int key;

    while (true)
    {
        terminal->print(" YES ", yes_no_x_pos, yes_no_y_pos, Colour::GREEN_BOLD, (result ? PRINT_FLAG_REVERSE : 0), yes_no_window);
        terminal->print(" NO ", yes_no_x_pos + 6, yes_no_y_pos, Colour::RED_BOLD, (result ? 0 : PRINT_FLAG_REVERSE), yes_no_window);
        terminal->flip();
        key = game->get_key();
        if (key == Key::RESIZE) return Key::RESIZE;
        else if (game->is_key_west(key)) result = true;
        else if (game->is_key_east(key)) result = false;
        else if (key == 'y' || key == 'Y') return 'Y';
        else if (key == 'n' || key == 'N') return 'N';
        else if (key == Key::ENTER) return (result ? 'Y' : 'N');
    }
}

}   // namespace invictus
