// ui/ui.hpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_UI_HPP_
#define UI_UI_HPP_

#include <memory>


namespace invictus
{

class MessageLog;   // defined in ui/msglog.hpp
class Window;       // defined in terminal/window.hpp


class UI
{
public:
    static constexpr int    NEARBY_BAR_WIDTH =      22; // The horizontal width of the 'nearby' bar.
    static constexpr int    MESSAGE_LOG_HEIGHT =    10; // The height of the message log window.

            UI();               // Constructor, sets up UI elements.
            ~UI();              // Destructor, calls cleanup function.
    void    cleanup();          // Cleans up any sub-elements.
    void    dungeon_mode_ui(bool enable);   // Enables or disables the dungeon-mode UI.
    void    redraw_dungeon();   // Marks the dungeon view as needing to be redrawn.
    void    redraw_message_log();   // Marks the message log window as needing to be redrawn.
    void    redraw_nearby();    // Marks the nearby window as needing to be redrawn.
    void    render();           // Renders the UI elements, if needed.
    void    window_resized();   // The terminal window has been resized.

    const std::shared_ptr<Window>       dungeon_view() const;       // Gets a pointer to the dungeon view window.
    const std::shared_ptr<Window>       message_log_window() const; // Gets a pointer to the message log window.
    const std::shared_ptr<MessageLog>   msglog() const;             // Gets a pointer to the message log object.
    const std::shared_ptr<Window>       nearby_window() const;      // Gets a pointer to the nearby sidebar window.

private:
    void    generate_dungeon_view();    // Generates the dungeon view window.
    void    generate_message_log();     // Generates the message log window.
    void    generate_nearby_window();   // Generates the nearby sidebar window.

    bool    cleanup_done_;          // Has the cleanup routine already run once?
    bool    dungeon_needs_redraw_;  // Does the dungeon need re-rendering?
    std::shared_ptr<Window> dungeon_view_;      // The corner of the screen where we view the dungeon.
    std::shared_ptr<MessageLog> message_log_;   // The message log object.
    bool    message_log_needs_redraw_;          // Does the message log window need re-rendering?
    std::shared_ptr<Window> message_log_window_;    // The message log window.
    bool    nearby_needs_redraw_;   // Does the nearby window need re-rendering?
    std::shared_ptr<Window> nearby_window_;     // The nearby sidebar window.
};

}       // namespace invictus
#endif  // UI_UI_HPP_
