// core/terminal.hpp -- Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_TERMINAL_HPP_
#define CORE_TERMINAL_HPP_

#include <climits>
#include <cstdint>
#include <map>
#include <string>


enum class Colour : uint8_t { BLACK, BLACK_BOLD, RED, RED_BOLD, GREEN, GREEN_BOLD, YELLOW, YELLOW_BOLD, BLUE, BLUE_BOLD, MAGENTA, MAGENTA_BOLD, CYAN, CYAN_BOLD,
    WHITE, WHITE_BOLD };

enum Key { BACKSPACE = 8, TAB = 9, ENTER = 10, CR = 13, ESCAPE = 27, CLOSE = 256, RESIZE, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, DELETE, INSERT, HOME, END,
    PAGE_UP, PAGE_DOWN, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, KP0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9, UNKNOWN = USHRT_MAX };

class Terminal
{
public:
            Terminal();     // Sets up the Curses terminal.
            ~Terminal();    // Cleans up Curses, resets the terminal to its former state.
    void    cls();          // Clears the screen.
    void    flip();         // Updates the screen.
    int     get_key();      // Gets keyboard input from the user.
    void    print(std::string str, int x, int y, Colour col = Colour::WHITE);   // Prints a string at a given coordinate on the screen.
    void    put(char letter, int x, int y, Colour col = Colour::WHITE); // Prints a character at a given coordinate on the screen.

private:
    unsigned long   colour_pair_code(Colour col);   // Returns a colour pair code.

    bool    has_colour_;                // The terminal has colour support.
    int     size_x_, size_y_;           // The X,Y dimensions of the screen.

    static std::map<std::string, int>   escape_code_index_; // Hard-coded list of escape codes used by various terminals.
};

#endif  // CORE_TERMINAL_HPP_
