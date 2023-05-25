// terminal/terminal.hpp -- Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TERMINAL_TERMINAL_HPP_
#define TERMINAL_TERMINAL_HPP_

#include <climits>
#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "terminal/terminal-shared-defs.hpp"


namespace invictus
{

class Window;   // defined in terminal/window.hpp


class Terminal
{
public:
                Terminal();     // Sets up the Curses terminal.
                ~Terminal();    // Destructor, calls cleanup code.
    void        box(std::shared_ptr<Window> window = nullptr, Colour colour = Colour::NONE, unsigned int flags = 0);    // Draws a box around the edge of a Window.
    void        cleanup();      // Cleans up Curses, resets the terminal to its former state.
    void        clear_line(std::shared_ptr<Window> window = nullptr);   // Clears the current line.
    void        cls(std::shared_ptr<Window> window = nullptr);          // Clears the screen.
    void        flip();     // Updates the screen.
    void        flush();    // Flushes the input buffer.
    uint16_t    get_cols(std::shared_ptr<Window> window = nullptr); // Gets the number of columns available on the screen right now.
    uint16_t    get_cursor_x(std::shared_ptr<Window> window = nullptr); // Gets the current cursor X coordinate.
    uint16_t    get_cursor_y(std::shared_ptr<Window> window = nullptr); // Gets the current cursor Y coordinate.
    int         get_key(std::shared_ptr<Window> window = nullptr);      // Gets keyboard input from the user.
    uint16_t    get_midcol(std::shared_ptr<Window> window = nullptr);   // Gets the central column of the specified Window.
    uint16_t    get_midrow(std::shared_ptr<Window> window = nullptr);   // Gets the central row of the specified Window.
    uint16_t    get_rows(std::shared_ptr<Window> window = nullptr);     // Gets the number of rows available on the screen right now.
    std::string get_string(std::shared_ptr<Window> window = nullptr);   // C++ std::string wrapper around the PDCurses wgetnstr() function.
    void        move_cursor(int x, int y, std::shared_ptr<Window> window = nullptr);    // Moves the cursor to the given coordinates.
                                                                                        // -1 for either coordinate retains its current position on that axis.
                // Prints a string at a given coordinate on the screen.
    void        print(std::string str, int x, int y, Colour col = Colour::WHITE, unsigned int flags = 0, std::shared_ptr<Window> window = nullptr);
                // Prints a character at a given coordinate on the screen.
    void        put(uint32_t letter, int x, int y, Colour col = Colour::WHITE, unsigned int flags = 0, std::shared_ptr<Window> window = nullptr);
    void        set_cursor(bool enabled);   // Turns the cursor on or off.

private:
    unsigned long   colour_pair_code(Colour col);   // Returns a colour pair code.

    bool    cleanup_done_;  // Has the cleanup routine already run once?
    int     cursor_state_;  // The current state of the cursor.
    bool    has_colour_;    // The terminal has colour support.
    bool    initialized_;   // Has Curses been initialized?

    static std::map<std::string, int>   escape_code_index_; // Hard-coded list of escape codes used by various terminals.
};

}       // namespace invictus
#endif  // TERMINAL_TERMINAL_HPP_
