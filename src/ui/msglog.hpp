// ui/msglog.hpp -- The message log, which displays important information to the player.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_MSGLOG_HPP_
#define UI_MSGLOG_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>


namespace invictus
{

class Timer;    // defined in util/timer.hpp


class MessageLog
{
public:
            MessageLog();       // Constructor, sets up the message log window.
    void    amend(const std::string &str);  // Amends the last message, adding additional text.
    void    blank_line();       // Prints a blank line.
    void    message(std::string msg, unsigned char awaken_chance = 0);  // Adds a message to the output buffer.
    void    render();           // Draws the message log on the screen.
    void    screen_resized();   // Lets the message log know the screen size has changed.

private:
    void    process_output_buffer();    // Processes the output buffer after an update.
    void    purge_buffer();     // Clears the entire output buffer.
    void    reset_buffer_pos(); // Resets the output buffer position.

    unsigned int                buffer_pos_;        // The position of the output buffer.
    std::vector<std::string>    output_prc_;        // The nicely processed output buffer, ready for rendering.
    std::vector<bool>           output_prc_fade_;   // The colour fade tags on older messages.
    std::vector<std::string>    output_raw_;        // The raw, unprocessed output buffer.
    std::vector<bool>           output_raw_fade_;   // The colour fade tags on older messages.
    std::shared_ptr<Timer>      timer_;             // The timer for determining when old messages are stale.

friend class SaveLoad;
};

}       // namespace invictus
#endif  // UI_MSGLOG_HPP_
