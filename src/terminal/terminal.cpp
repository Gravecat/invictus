// core/terminal.cpp -- Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <curses.h>
#include <fstream>
#include <panel.h>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "core/strx.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"


// Sets up the Curses terminal.
Terminal::Terminal() : cursor_state_(1), has_colour_(false), initialized_(false)
{
    initscr();  // Curses initialization
    cbreak();   // Disable line-buffering.
    if (has_colors())
    {
        start_color();  // Enables use of colour
        has_colour_ = true;
    }
    noecho();               // Disables keyboard input being printed to the screen
    keypad(stdscr, true);   // Enables the use of the numeric keypad
    set_cursor(false);      // Hides the blinking cursor

    if (has_colour_)
    {
        init_pair(1, COLOR_BLACK, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_CYAN, COLOR_BLACK);
        init_pair(8, COLOR_WHITE, COLOR_BLACK);
    }

#ifdef INVICTUS_TARGET_WINDOWS
    PDC_set_title("Morior Invictus prototype");
    PDC_set_blink(true);
#endif
    core()->guru()->console_ready(true);
    core()->guru()->log("Curses terminal is up and running.");
    initialized_ = true;
}

// Cleans up Curses, resets the terminal to its former state.
Terminal::~Terminal() { cleanup(); }

// Draws a box around the edge of a Window.
void Terminal::box(std::shared_ptr<Window> window, Colour colour, unsigned int flags)
{
    WINDOW *win = (window ? window->win() : stdscr);
    const bool bold = ((flags & PRINT_FLAG_BOLD) == PRINT_FLAG_BOLD);
    const bool reverse = ((flags & PRINT_FLAG_REVERSE) == PRINT_FLAG_REVERSE);
    const bool blink = ((flags & PRINT_FLAG_BLINK) == PRINT_FLAG_BLINK);

    unsigned int colour_flags = 0;
    if (bold) colour_flags |= A_BOLD;
    if (reverse) colour_flags |= A_REVERSE;
    if (blink) colour_flags |= A_BLINK;

    if (colour != Colour::NONE) wattron(win, colour_pair_code(colour) | colour_flags);
    ::box(win, 0, 0);
    if (colour != Colour::NONE) wattroff(win, colour_pair_code(colour) | colour_flags);
}

// Attempts to gracefully clean up Curses.
void Terminal::cleanup()
{
    if (!initialized_) return;
    echo();                 // Re-enables keyboard input being printed to the screen (normal console behaviour)
    keypad(stdscr, false);  // Disables the numeric keypad (it's off by default)
    curs_set(1);            // Re-enables the blinking cursor
    nocbreak();             // Re-enables line buffering.
    endwin();               // Cleans up Curses internally.
    initialized_ = false;
}

// Clears the current line.
void clear_line(std::shared_ptr<Window> window)
{
    WINDOW *win = (window ? window->win() : stdscr);
    wclrtoeol(win);
}

// Clears the screen.
void Terminal::cls(std::shared_ptr<Window> window)
{
#ifdef PDCURSES
    // Workaround to deal with PDCurses' lack of an inbuilt SIGWINCH handler.
    if (is_termresized())
    {
        resize_term(0, 0);
        if (get_cols() < 80 || get_rows() < 24) resize_term(24, 80);
        curs_set(cursor_state_);
    }
#endif
    if (!window) clear();
    else wclear(window->win());
}

// Returns a colour pair code.
unsigned long Terminal::colour_pair_code(Colour col)
{
    switch(col)
    {
        case Colour::BLACK: return COLOR_PAIR(1);
        case Colour::BLACK_BOLD: return COLOR_PAIR(1) | A_BOLD;
        case Colour::RED: return COLOR_PAIR(2);
        case Colour::RED_BOLD: return COLOR_PAIR(2) | A_BOLD;
        case Colour::GREEN: return COLOR_PAIR(3);
        case Colour::GREEN_BOLD: return COLOR_PAIR(3) | A_BOLD;
        case Colour::YELLOW: return COLOR_PAIR(4);
        case Colour::YELLOW_BOLD: return COLOR_PAIR(4) | A_BOLD;
        case Colour::BLUE: return COLOR_PAIR(5);
        case Colour::BLUE_BOLD: return COLOR_PAIR(5) | A_BOLD;
        case Colour::MAGENTA: return COLOR_PAIR(6);
        case Colour::MAGENTA_BOLD: return COLOR_PAIR(6) | A_BOLD;
        case Colour::CYAN: return COLOR_PAIR(7);
        case Colour::CYAN_BOLD: return COLOR_PAIR(7) | A_BOLD;
        case Colour::WHITE: return COLOR_PAIR(8);
        case Colour::WHITE_BOLD: return COLOR_PAIR(8) | A_BOLD;
        default: return 0;
    }
}

// Updates the screen.
void Terminal::flip()
{
    if (get_cols() < 80 || get_rows() < 24) resize_term(24, 80);
    update_panels();
    refresh();
}

// Flushes the input buffer.
void Terminal::flush() { flushinp(); }

// Gets the number of columns available on the screen right now.
uint16_t Terminal::get_cols(std::shared_ptr<Window> window)
{
    if (window) return window->get_width();
    else return getmaxx(stdscr);
}

// Gets the current cursor X coordinate.
uint16_t Terminal::get_cursor_x(std::shared_ptr<Window> window)
{
    WINDOW *win = (window ? window->win() : stdscr);
    return getcurx(win);
}

// Gets the current cursor Y coordinate.
uint16_t Terminal::get_cursor_y(std::shared_ptr<Window> window)
{
    WINDOW *win = (window ? window->win() : stdscr);
    return getcury(win);
}

// Gets keyboard input from the user.
int Terminal::get_key(std::shared_ptr<Window> window)
{
    WINDOW *win = (window ? window->win() : stdscr);
    int the_key = wgetch(win);

    if (the_key == 27)
    {
        std::string key_str = "\x1b";
        nodelay(win, true);
        do
        {
            the_key = wgetch(win);
            if (the_key > 0 && the_key != Key::ESCAPE) key_str += std::string(1, static_cast<char>(the_key));
        } while (the_key > 0);
        nodelay(win, false);
        auto result = escape_code_index_.find(key_str);
        if (result == escape_code_index_.end())
        {
            std::ofstream file("out.txt", std::ios::app);
            file << key_str << std::endl;
            file.close();
            return Key::UNKNOWN;
        }
        else return result->second;
    }

    if ((the_key >= 5 && the_key <= 26) || (the_key >= ' ' && the_key <= '~')) return the_key;
    else switch(the_key)
    {
        case KEY_RESIZE:    // Window resized event.
        {
            resize_term(0, 0);
            if (get_cols() < 80 || get_rows() < 24) resize_term(24, 80);
            curs_set(cursor_state_);
            return Key::RESIZE;
        }
        case 1: case 2: return the_key;
        case 3: case 4: return Key::CLOSE;
        case KEY_BACKSPACE: return Key::BACKSPACE;
        case KEY_DC: return Key::DELETE;
        case KEY_DOWN: return Key::ARROW_DOWN;
        case KEY_END: return Key::END;
        case KEY_ENTER: return Key::ENTER;
        case KEY_F0 + 1: return Key::F1;
        case KEY_F0 + 2: return Key::F2;
        case KEY_F0 + 3: return Key::F3;
        case KEY_F0 + 4: return Key::F4;
        case KEY_F0 + 5: return Key::F5;
        case KEY_F0 + 6: return Key::F6;
        case KEY_F0 + 7: return Key::F7;
        case KEY_F0 + 8: return Key::F8;
        case KEY_F0 + 9: return Key::F9;
        case KEY_F0 + 10: return Key::F10;
        case KEY_F0 + 11: return Key::F11;
        case KEY_F0 + 12: return Key::F12;
        case KEY_HOME: return Key::HOME;
        case KEY_IC: return Key::INSERT;
        case KEY_LEFT: return Key::ARROW_LEFT;
        case KEY_NPAGE: return Key::PAGE_DOWN;
        case KEY_PPAGE: return Key::PAGE_UP;
        case KEY_RIGHT: return Key::ARROW_RIGHT;
        case KEY_UP: return Key::ARROW_UP;
#ifdef INVICTUS_TARGET_WINDOWS
        case KEY_A1: return Key::KP7;
        case KEY_A2: return Key::KP8;
        case KEY_A3: return Key::KP9;
        case KEY_B1: return Key::KP4;
        case KEY_B2: return Key::KP5;
        case KEY_B3: return Key::KP6;
        case KEY_C1: return Key::KP1;
        case KEY_C2: return Key::KP2;
        case KEY_C3: return Key::KP3;
        case PAD0: return Key::KP0;
        case PADENTER: return Key::ENTER;
        case PADSTOP: return '.';
        case PADMINUS: return '-';
        case PADPLUS: return '+';
        case PADSLASH: return '/';
        case PADSTAR: return '*';
#endif
        default: return Key::UNKNOWN;
    }
}

// Gets the central column of the specified Window.
uint16_t Terminal::get_midcol(std::shared_ptr<Window> window)
{
    if (window) return window->get_width() / 2;
    else return getmaxx(stdscr) / 2;
}

// Gets the central row of the specified Window.
uint16_t Terminal::get_midrow(std::shared_ptr<Window> window)
{
    if (window) return window->get_height() / 2;
    else return getmaxy(stdscr) / 2;
}

// Gets the number of rows available on the screen right now.
uint16_t Terminal::get_rows(std::shared_ptr<Window> window)
{
    if (window) return window->get_height();
    else return getmaxy(stdscr);
}

// C++ std::string wrapper around the PDCurses wgetnstr() function.
std::string get_string(std::shared_ptr<Window> window)
{
    WINDOW *win = (window ? window->win() : stdscr);
    char buffer[256];
    wgetnstr(win, buffer, 255);
    return buffer;
}

// Moves the cursor to the given coordinates; -1 for either coordinate retains its current position on that axis.
void Terminal::move_cursor(int x, int y, std::shared_ptr<Window> window)
{
    if (x == -1 && y == -1) return;
    WINDOW *win = (window ? window->win() : stdscr);
    const int old_x = get_cursor_x(window);
    const int old_y = get_cursor_y(window);
    if (x == -1) x = old_x;
    if (y == -1) y = old_y;
    wmove(win, y, x);
}

// Prints a string at a given coordinate on the screen.
void Terminal::print(std::string str, int x, int y, Colour col, unsigned int flags, std::shared_ptr<Window> window)
{
    if (!str.size()) return;
    WINDOW *win = (window ? window->win() : stdscr);
    
    unsigned int colour_flags = 0;
    if ((flags & PRINT_FLAG_BOLD) == PRINT_FLAG_BOLD) colour_flags |= A_BOLD;
    if ((flags & PRINT_FLAG_REVERSE) == PRINT_FLAG_REVERSE) colour_flags |= A_REVERSE;
    if ((flags & PRINT_FLAG_BLINK) == PRINT_FLAG_BLINK) colour_flags |= A_BLINK;

    if (str.find("{") == std::string::npos)
    {
        // If no colour codes are present, this is fairly easy.
        const unsigned long ansi_code = colour_pair_code(col);
        if (has_colour_) wattron(win, ansi_code | colour_flags);
        mvwprintw(win, y, x, str.c_str());
        if (has_colour_) wattroff(win, ansi_code | colour_flags);
        return;
    }

    // Colour codes need to be parsed. Curses doesn't support multiple colour codes in a single printw(), so we're gonna have to get creative.

    if (str[0] != '{') str = "{w}" + str;   // If there isn't a colour tag specified at the start, start it with white.

    while (str.size())
    {
        std::string first_word;
        size_t tag_pos = str.substr(1).find_first_of('{');  // We skip ahead one char, to ignore the opening brace. We want to find the *next* opening brace.
        if (tag_pos != std::string::npos)
        {
            first_word = str.substr(0, tag_pos + 1);
            str = str.substr(tag_pos + 1);
        }
        else
        {
            first_word = str;
            str = "";
        }

        while (first_word.size() >= 3 && first_word[0] == '{' && first_word[2] == '}')
        {
            const std::string tag = first_word.substr(0, 3);
            first_word = first_word.substr(3);
            switch(tag[1])
            {
                case 'b': col = Colour::BLACK; break;
                case 'B': col = Colour::BLACK_BOLD; break;
                case 'r': col = Colour::RED; break;
                case 'R': col = Colour::RED_BOLD; break;
                case 'g': col = Colour::GREEN; break;
                case 'G': col = Colour::GREEN_BOLD; break;
                case 'y': col = Colour::YELLOW; break;
                case 'Y': col = Colour::YELLOW_BOLD; break;
                case 'u': col = Colour::BLUE; break;
                case 'U': col = Colour::BLUE_BOLD; break;
                case 'm': col = Colour::MAGENTA; break;
                case 'M': col = Colour::MAGENTA_BOLD; break;
                case 'c': col = Colour::CYAN; break;
                case 'C': col = Colour::CYAN_BOLD; break;
                case 'w': col = Colour::WHITE; break;
                case 'W': col = Colour::WHITE_BOLD; break;
            }
        }

        const unsigned long ansi_code = colour_pair_code(col);
        if (has_colour_) wattron(win, ansi_code | colour_flags);
        const unsigned int first_word_size = first_word.size();
        StrX::find_and_replace(first_word, "%", "%%");
        mvwprintw(win, y, x, first_word.c_str());
        if (has_colour_) wattroff(win, ansi_code | colour_flags);
        x += first_word_size;
    }
}

// Prints a character at a given coordinate on the screen.
void Terminal::put(uint32_t letter, int x, int y, Colour col, unsigned int flags, std::shared_ptr<Window> window)
{
    unsigned int colour_flags = 0;
    if ((flags & PRINT_FLAG_BOLD) == PRINT_FLAG_BOLD) colour_flags |= A_BOLD;
    if ((flags & PRINT_FLAG_REVERSE) == PRINT_FLAG_REVERSE) colour_flags |= A_REVERSE;
    if ((flags & PRINT_FLAG_BLINK) == PRINT_FLAG_BLINK) colour_flags |= A_BLINK;
    WINDOW *win = (window ? window->win() : stdscr);

    if (letter > 255)
    {
        switch(static_cast<Glyph>(letter))
        {
            case Glyph::ULCORNER: letter = ACS_ULCORNER; break;
            case Glyph::LLCORNER: letter = ACS_LLCORNER; break;
            case Glyph::URCORNER: letter = ACS_URCORNER; break;
            case Glyph::LRCORNER: letter = ACS_LRCORNER; break;
            case Glyph::RTEE: letter = ACS_RTEE; break;
            case Glyph::LTEE: letter = ACS_LTEE; break;
            case Glyph::BTEE: letter = ACS_BTEE; break;
            case Glyph::TTEE: letter = ACS_TTEE; break;
            case Glyph::HLINE: letter = ACS_HLINE; break;
            case Glyph::VLINE: letter = ACS_VLINE; break;
            case Glyph::PLUS: letter = ACS_PLUS; break;
            case Glyph::S1: letter = ACS_S1; break;
            case Glyph::S9: letter = ACS_S9; break;
            case Glyph::DIAMOND: letter = ACS_DIAMOND; break;
            case Glyph::CKBOARD: letter = ACS_CKBOARD; break;
            case Glyph::DEGREE: letter = ACS_DEGREE; break;
            case Glyph::PLMINUS: letter = ACS_PLMINUS; break;
            case Glyph::BULLET: letter = ACS_BULLET; break;
            case Glyph::LARROW: letter = ACS_LARROW; break;
            case Glyph::RARROW: letter = ACS_RARROW; break;
            case Glyph::DARROW: letter = ACS_DARROW; break;
            case Glyph::UARROW: letter = ACS_UARROW; break;
            case Glyph::BOARD: letter = ACS_BOARD; break;
            case Glyph::LANTERN: letter = ACS_LANTERN; break;
            case Glyph::BLOCK: letter = ACS_BLOCK; break;
            case Glyph::S3: letter = ACS_S3; break;
            case Glyph::S7: letter = ACS_S7; break;
            case Glyph::LEQUAL: letter = ACS_LEQUAL; break;
            case Glyph::GEQUAL: letter = ACS_GEQUAL; break;
            case Glyph::PI: letter = ACS_PI; break;
            case Glyph::NEQUAL: letter = ACS_NEQUAL; break;
            case Glyph::STERLING: letter = ACS_STERLING; break;
        }
    }
    
    const unsigned long ansi_code = colour_pair_code(col);
    if (has_colour_) wattron(win, ansi_code | colour_flags);
    mvwaddch(win, y, x, letter);
    if (has_colour_) wattroff(win, ansi_code | colour_flags);
}

// Turns the cursor on or off.
void Terminal::set_cursor(bool enabled)
{
    if (enabled)
    {
        cursor_state_ = 2;
        curs_set(2);
    }
    else
    {
        cursor_state_ = 0;
        curs_set(0);
    }
}
