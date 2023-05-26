// terminal/window.hpp -- The Window class, part of the Terminal subsystem, allowing easier management of Curses windows and panels.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TERMINAL_WINDOW_HPP_
#define TERMINAL_WINDOW_HPP_

#include <memory>

#include "terminal/terminal-shared-defs.hpp"


namespace invictus
{

class Window
{
public:
                Window(int width, int height, int new_x = 0, int new_y = 0, bool new_border = false);
                ~Window();
    uint16_t    get_height() const; // Read-only access to the Window's height.
    uint16_t    get_width() const;  // Read-only access to the Window's width.
    void        move(int new_x, int new_y); // Moves this Window's underlying panel to new coordinates.
    void        redraw_border(Colour col = Colour::NONE, uint32_t flags = 0);   // Re-renders the border around this Window, if any.
    void        set_visible(bool vis);      // Set this Window's panel as visible or invisible.
    WINDOW*     win() const;    // Returns a pointer to the WINDOW struct.

private:
    std::shared_ptr<Window> border_ptr_;    // If a border is present, this is the underlying border Window.
    uint16_t    height_;        // The height of this Window.
    PANEL*      panel_ptr_;     // A pointer to the underlying PANEL struct.
    uint16_t    width_;         // The width of this Window.
    WINDOW*     window_ptr_;    // A pointer to the underlying WINDOW struct.
    int         x_, y_;         // The screen coordinates of this Window.
};

}       // namespace invictus
#endif  // TERMINAL_WINDOW_HPP_
