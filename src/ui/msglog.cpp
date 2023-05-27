// ui/msglog.cpp -- The message log, which displays important information to the player.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "tune/message-log.hpp"
#include "ui/msglog.hpp"
#include "ui/ui.hpp"
#include "util/random.hpp"
#include "util/strx.hpp"
#include "util/timer.hpp"


namespace invictus
{

// Constructor, sets up the message log window.
MessageLog::MessageLog() : buffer_pos_(0), timer_(std::make_shared<Timer>())
{
    // Empty the log to begin with, so new messages show at the bottom.
    output_raw_.resize(MESSAGE_LOG_HEIGHT - 2);
    output_raw_fade_.resize(MESSAGE_LOG_HEIGHT - 2);
}

// Amends the last message, adding additional text.
void MessageLog::amend(const std::string &str)
{
    if (!output_raw_.size())
    {
        message(str);
        return;
    }
    output_raw_.at(output_raw_.size() - 1) += str;
    process_output_buffer();
}

// Prints a blank line.
void MessageLog::blank_line() { message(""); }

// Adds a message to the log!
void MessageLog::message(std::string msg, unsigned char awaken_chance)
{
    // Check to see if this is a new message, and to mark other messages as old.
    if (timer_->elapsed() >= MESSAGE_LOG_FADE_TIMER)
    {
        timer_->reset();
        for (unsigned int i = 0; i < output_raw_fade_.size(); i++)
            output_raw_fade_.at(i) = true;
    }

    // Split the message if we detect a \n specified in there.
    size_t found = msg.find("\n");
    if (found != std::string::npos)
    {
        message(msg.substr(0, found), awaken_chance);
        message(msg.substr(found + 1), awaken_chance);
        return;
    }

    // Check to see if this is something that will wake up the sleeping player.
    const bool sleeping = false;    // todo: When player resting/sleeping is implemented, do a check for a sleeping player here.
    if (sleeping && awaken_chance > 0 && awaken_chance != AWAKEN_CHANCE_ALWAYS_SHOW_BUT_NEVER_WAKE)
    {
        if (Random::rng(100) <= awaken_chance) { }  // todo: When player sleeping/resting is implemented, trigger a wake-up here.
        else return;
    }

    output_raw_.push_back(msg);
    output_raw_fade_.push_back(false);
    process_output_buffer();    // Reprocess the text, to make sure it's all where it should be.
    core()->game()->ui()->redraw_message_log(); // Tells the UI that the message log window should be redrawn.
}

// Processes the output buffer after an update.
void MessageLog::process_output_buffer()
{
    // Trim the output buffer if necessary.
    while (output_raw_.size() > MESSAGE_LOG_OUTPUT_BUFFER_MAX)
    {
        output_raw_.erase(output_raw_.begin());
        output_raw_fade_.erase(output_raw_fade_.begin());
    }

    // Clear the processed buffer.
    output_prc_.clear();
    output_prc_fade_.clear();
    if (!output_raw_.size()) return;

    // Process the output buffer.
    int term_width = core()->game()->ui()->message_log_window()->get_width();
    for (unsigned int i = 0; i < output_raw_.size(); i++)
    {
        std::vector<std::string> line_vec = StrX::string_explode_colour(output_raw_.at(i), term_width - 2);
        for (auto line : line_vec)
        {
            output_prc_.push_back(line);
            output_prc_fade_.push_back(output_raw_fade_.at(i));
        }
    }

    // Reset the buffer position if needed.
    reset_buffer_pos();
}

// Clears the entire output buffer.
void MessageLog::purge_buffer()
{
    output_raw_.clear();
    output_raw_fade_.clear();
    output_prc_.clear();
    output_prc_fade_.clear();
    buffer_pos_ = 0;
    timer_->reset();
}

// Resets the output buffer position.
void MessageLog::reset_buffer_pos()
{
    buffer_pos_ = 0;
    const unsigned int height = MESSAGE_LOG_HEIGHT - 2;
    if (output_prc_.size() > height) buffer_pos_ = output_prc_.size() - height;
}

// Draws the message log on the screen.
void MessageLog::render()
{
    auto terminal = core()->terminal();
    auto ui = core()->game()->ui();
    auto msg_window = ui->message_log_window();
    terminal->box(msg_window, Colour::WHITE);

    if (output_prc_.size())
    {
        unsigned int end = output_prc_.size();
        if (end - buffer_pos_ > MESSAGE_LOG_HEIGHT - 2) end = buffer_pos_ + MESSAGE_LOG_HEIGHT - 2;
        for (unsigned int i = buffer_pos_; i < end; i++)
            terminal->print(output_prc_.at(i), 1, i - buffer_pos_ + 1, Colour::WHITE, output_prc_fade_.at(i) ? PRINT_FLAG_DARK : PRINT_FLAG_BOLD, msg_window);
    }
}

// Lets the message log know the screen size has changed.
void MessageLog::screen_resized() { process_output_buffer(); }

}   // namespace invictus
