// ui/ui.hpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_UI_HPP_
#define UI_UI_HPP_

namespace invictus
{

class UI
{
public:
    static constexpr int    NEARBY_BAR_WIDTH =      22; // The horizontal width of the 'nearby' bar.
    static constexpr int    MESSAGE_LOG_HEIGHT =    10; // The height of the message log window.

            UI();       // Constructor, sets up UI elements.
            ~UI();      // Destructor, calls cleanup function.
    void    cleanup();  // Cleans up any sub-elements.
    void    render();   // Renders the UI elements, if needed.
    void    visible_area(int *x, int *y);   // Returns the visible area not covered by the UI.

private:
    bool    cleanup_done_;  // Has the cleanup routine already run once?
};

}       // namespace invictus
#endif  // UI_UI_HPP_
