// core/terminal.cpp -- Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <curses.h>

#include "core/strx.hpp"
#include "core/terminal.hpp"


// Sets up the Curses terminal.
Terminal::Terminal() : has_colour_(false)
{
    initscr();              // Curses initialization
    if (has_colors())
    {
            start_color();  // Enables use of colour
            has_colour_ = true;
    }
    noecho();               // Disables keyboard input being printed to the screen
    keypad(stdscr, true);   // Enables the use of the numeric keypad
    curs_set(0);            // Hides the blinking cursor

    // Get the screen size.
    getmaxyx(stdscr, size_y_, size_x_);

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
#endif
}

// Cleans up Curses, resets the terminal to its former state.
Terminal::~Terminal()
{
    echo();                 // Re-enables keyboard input being printed to the screen (normal console behaviour)
    keypad(stdscr, false);  // Disables the numeric keypad (it's off by default)
    curs_set(1);            // Re-enables the blinking cursor
    endwin();
}

// Clears the screen.
void Terminal::cls() { clear(); }

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
void Terminal::flip() { refresh(); }

// Gets keyboard input from the user.
uint16_t Terminal::get_key()
{
    int key = getch();
    switch(key)
    {
        case 3: case 4: return Key::CLOSE;  // Ctrl-C or Ctrl-D close the console window.
        case KEY_RESIZE:    // Window resized event.
        {
            resize_term(0, 0);
            curs_set(0);
            getmaxyx(stdscr, size_y_, size_x_);
            return Key::RESIZE;
        }
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
    }
    if (key > 255 || key < 0) return Key::UNKNOWN;  // Any other unrecognized keys are just returned as unknown.
    return key;
}

// Prints a string at a given coordinate on the screen.
void Terminal::print(std::string str, int x, int y, Colour col)
{
    if (!str.size()) return;
    if (str.find("{") == std::string::npos)
    {
        // If no colour codes are present, this is fairly easy.
        const unsigned long ansi_code = colour_pair_code(col);
        if (has_colour_) attron(ansi_code);
        mvprintw(y, x, str.c_str());
        if (has_colour_) attroff(ansi_code);
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
        if (has_colour_) attron(ansi_code);
        const unsigned int first_word_size = first_word.size();
        StrX::find_and_replace(first_word, "%", "%%");
        mvprintw(y, x, first_word.c_str());
        if (has_colour_) attroff(ansi_code);
        x += first_word_size;
    }
}

// Prints a character at a given coordinate on the screen.
void Terminal::put(char letter, int x, int y, Colour col)
{
    const unsigned long ansi_code = colour_pair_code(col);
    if (has_colour_) attron(ansi_code);
    mvaddch(y, x, letter);
    if (has_colour_) attroff(ansi_code);
}
