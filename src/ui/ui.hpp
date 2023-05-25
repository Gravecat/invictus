// ui/ui.hpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_UI_HPP_
#define UI_UI_HPP_

#include <memory>


namespace invictus
{

class Window;   // defined in terminal/window.hpp


class UI
{
public:
    static constexpr int    NEARBY_BAR_WIDTH =      22; // The horizontal width of the 'nearby' bar.
    static constexpr int    MESSAGE_LOG_HEIGHT =    10; // The height of the message log window.

            UI();       // Constructor, sets up UI elements.
            ~UI();      // Destructor, calls cleanup function.
    void    cleanup();  // Cleans up any sub-elements.
    void    render();   // Renders the UI elements, if needed.
    void    window_resized();   // The terminal window has been resized.

    const std::shared_ptr<Window>   dungeon_view() const;   // Gets a pointer to the dungeon view window.

private:
    void    generate_dungeon_view();    // Generates the dungeon view window.

    bool    cleanup_done_;          // Has the cleanup routine already run once?
    bool    dungeon_needs_redraw_;  // Does the dungeon need re-rendering?
    std::shared_ptr<Window> dungeon_view_;  // The corner of the screen where we view the dungeon.
};

}       // namespace invictus
#endif  // UI_UI_HPP_
