// core/terminal.cpp -- Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <curses.h>
#include <string>
#include <vector>

#include <fstream>

#include "core/strx.hpp"
#include "core/terminal.hpp"


std::map<std::string, int> Terminal::escape_code_index_ = {
    { "\x1b", Key::ESCAPE },
    { "\x1b[11^", Key::F1 },
    { "\x1b[11~", Key::F1 },
    { "\x1b[12^", Key::F2 },
    { "\x1b[12~", Key::F2 },
    { "\x1b[13^", Key::F3 },
    { "\x1b[13~", Key::F3 },
    { "\x1b[14^", Key::F4 },
    { "\x1b[14~", Key::F4 },
    { "\x1b[15;2~", Key::F5 },
    { "\x1b[15;3~", Key::F5 },
    { "\x1b[15;4~", Key::F5 },
    { "\x1b[15;5~", Key::F5 },
    { "\x1b[15;6~", Key::F5 },
    { "\x1b[15;7~", Key::F5 },
    { "\x1b[15;8~", Key::F5 },
    { "\x1b[15^", Key::F5 },
    { "\x1b[15~", Key::F5 },
    { "\x1b[17;2~", Key::F6 },
    { "\x1b[17;3~", Key::F6 },
    { "\x1b[17;4~", Key::F6 },
    { "\x1b[17;5~", Key::F6 },
    { "\x1b[17;6~", Key::F6 },
    { "\x1b[17;7~", Key::F6 },
    { "\x1b[17;8~", Key::F6 },
    { "\x1b[17^", Key::F6 },
    { "\x1b[17~", Key::F6 },
    { "\x1b[18;2~", Key::F7 },
    { "\x1b[18;3~", Key::F7 },
    { "\x1b[18;4~", Key::F7 },
    { "\x1b[18;5~", Key::F7 },
    { "\x1b[18;6~", Key::F7 },
    { "\x1b[18;7~", Key::F7 },
    { "\x1b[18;8~", Key::F7 },
    { "\x1b[18^", Key::F7 },
    { "\x1b[18~", Key::F7 },
    { "\x1b[19;2~", Key::F8 },
    { "\x1b[19;3~", Key::F8 },
    { "\x1b[19;4~", Key::F8 },
    { "\x1b[19;5~", Key::F8 },
    { "\x1b[19;6~", Key::F8 },
    { "\x1b[19;7~", Key::F8 },
    { "\x1b[19;8~", Key::F8 },
    { "\x1b[19^", Key::F8 },
    { "\x1b[19~", Key::F8 },
    { "\x1b[1;2A", Key::ARROW_UP },
    { "\x1b[1;2B", Key::ARROW_DOWN },
    { "\x1b[1;2C", Key::ARROW_RIGHT },
    { "\x1b[1;2D", Key::ARROW_LEFT },
    { "\x1b[1;2F", Key::END },
    { "\x1b[1;2H", Key::HOME },
    { "\x1b[1;2P", Key::F1 },
    { "\x1b[1;2Q", Key::F2 },
    { "\x1b[1;2R", Key::F3 },
    { "\x1b[1;2S", Key::F4 },
    { "\x1b[1;3A", Key::ARROW_UP },
    { "\x1b[1;3B", Key::ARROW_DOWN },
    { "\x1b[1;3C", Key::ARROW_RIGHT },
    { "\x1b[1;3D", Key::ARROW_LEFT },
    { "\x1b[1;3F", Key::END },
    { "\x1b[1;3H", Key::HOME },
    { "\x1b[1;3P", Key::F1 },
    { "\x1b[1;3Q", Key::F2 },
    { "\x1b[1;3R", Key::F3 },
    { "\x1b[1;3S", Key::F4 },
    { "\x1b[1;4A", Key::ARROW_UP },
    { "\x1b[1;4B", Key::ARROW_DOWN },
    { "\x1b[1;4C", Key::ARROW_RIGHT },
    { "\x1b[1;4D", Key::ARROW_LEFT },
    { "\x1b[1;4F", Key::END },
    { "\x1b[1;4H", Key::HOME },
    { "\x1b[1;4P", Key::F1 },
    { "\x1b[1;4Q", Key::F2 },
    { "\x1b[1;4R", Key::F3 },
    { "\x1b[1;4S", Key::F4 },
    { "\x1b[1;5A", Key::ARROW_UP },
    { "\x1b[1;5B", Key::ARROW_DOWN },
    { "\x1b[1;5C", Key::ARROW_RIGHT },
    { "\x1b[1;5D", Key::ARROW_LEFT },
    { "\x1b[1;5F", Key::END },
    { "\x1b[1;5H", Key::HOME },
    { "\x1b[1;5P", Key::F1 },
    { "\x1b[1;5Q", Key::F2 },
    { "\x1b[1;5R", Key::F3 },
    { "\x1b[1;5S", Key::F4 },
    { "\x1b[1;6A", Key::ARROW_UP },
    { "\x1b[1;6B", Key::ARROW_DOWN },
    { "\x1b[1;6C", Key::ARROW_RIGHT },
    { "\x1b[1;6D", Key::ARROW_LEFT },
    { "\x1b[1;6F", Key::END },
    { "\x1b[1;6H", Key::HOME },
    { "\x1b[1;6P", Key::F1 },
    { "\x1b[1;6Q", Key::F2 },
    { "\x1b[1;6R", Key::F3 },
    { "\x1b[1;6S", Key::F4 },
    { "\x1b[1;7A", Key::ARROW_UP },
    { "\x1b[1;7B", Key::ARROW_DOWN },
    { "\x1b[1;7C", Key::ARROW_RIGHT },
    { "\x1b[1;7D", Key::ARROW_LEFT },
    { "\x1b[1;7F", Key::END },
    { "\x1b[1;7H", Key::HOME },
    { "\x1b[1;7P", Key::F1 },
    { "\x1b[1;7Q", Key::F2 },
    { "\x1b[1;7R", Key::F3 },
    { "\x1b[1;7S", Key::F4 },
    { "\x1b[1;8A", Key::ARROW_UP },
    { "\x1b[1;8B", Key::ARROW_DOWN },
    { "\x1b[1;8C", Key::ARROW_RIGHT },
    { "\x1b[1;8D", Key::ARROW_LEFT },
    { "\x1b[1;8F", Key::END },
    { "\x1b[1;8H", Key::HOME },
    { "\x1b[1;8P", Key::F1 },
    { "\x1b[1;8Q", Key::F2 },
    { "\x1b[1;8R", Key::F3 },
    { "\x1b[1;8S", Key::F4 },
    { "\x1b[2$", Key::INSERT },
    { "\x1b[20;2~", Key::F9 },
    { "\x1b[20;3~", Key::F9 },
    { "\x1b[20;4~", Key::F9 },
    { "\x1b[20;5~", Key::F9 },
    { "\x1b[20;6~", Key::F9 },
    { "\x1b[20;7~", Key::F9 },
    { "\x1b[20;8~", Key::F9 },
    { "\x1b[20^", Key::F9 },
    { "\x1b[20~", Key::F9 },
    { "\x1b[21;2~", Key::F10 },
    { "\x1b[21;3~", Key::F10 },
    { "\x1b[21;4~", Key::F10 },
    { "\x1b[21;5~", Key::F10 },
    { "\x1b[21;6~", Key::F10 },
    { "\x1b[21;7~", Key::F10 },
    { "\x1b[21;8~", Key::F10 },
    { "\x1b[21^", Key::F10 },
    { "\x1b[21~", Key::F10 },
    { "\x1b[23$", Key::F11 },
    { "\x1b[23;2~", Key::F11 },
    { "\x1b[23;3~", Key::F11 },
    { "\x1b[23;4~", Key::F11 },
    { "\x1b[23;5~", Key::F11 },
    { "\x1b[23;6~", Key::F11 },
    { "\x1b[23;7~", Key::F11 },
    { "\x1b[23;8~", Key::F11 },
    { "\x1b[23@", Key::F11 },
    { "\x1b[23^", Key::F1 },
    { "\x1b[23^", Key::F11 },
    { "\x1b[23~", Key::F1 },
    { "\x1b[23~", Key::F11 },
    { "\x1b[24$", Key::F12 },
    { "\x1b[24;2~", Key::F12 },
    { "\x1b[24;3~", Key::F12 },
    { "\x1b[24;4~", Key::F12 },
    { "\x1b[24;5~", Key::F12 },
    { "\x1b[24;6~", Key::F12 },
    { "\x1b[24;7~", Key::F12 },
    { "\x1b[24;8~", Key::F12 },
    { "\x1b[24@", Key::F12 },
    { "\x1b[24^", Key::F12 },
    { "\x1b[24^", Key::F2 },
    { "\x1b[24~", Key::F12 },
    { "\x1b[24~", Key::F2 },
    { "\x1b[25^", Key::F3 },
    { "\x1b[25~", Key::F3 },
    { "\x1b[26^", Key::F4 },
    { "\x1b[26~", Key::F4 },
    { "\x1b[28^", Key::F5 },
    { "\x1b[28~", Key::F5 },
    { "\x1b[29^", Key::F6 },
    { "\x1b[29~", Key::F6 },
    { "\x1b[2;2~", Key::INSERT },
    { "\x1b[2;3~", Key::INSERT },
    { "\x1b[2;4~", Key::INSERT },
    { "\x1b[2;5~", Key::INSERT },
    { "\x1b[2;6~", Key::INSERT },
    { "\x1b[2;7~", Key::INSERT },
    { "\x1b[2;8~", Key::INSERT },
    { "\x1b[2@", Key::INSERT },
    { "\x1b[2^", Key::INSERT },
    { "\x1b[2~", Key::INSERT },
    { "\x1b[3$", Key::DELETE },
    { "\x1b[31^", Key::F7 },
    { "\x1b[31~", Key::F7 },
    { "\x1b[32^", Key::F8 },
    { "\x1b[32~", Key::F8 },
    { "\x1b[33^", Key::F9 },
    { "\x1b[33~", Key::F9 },
    { "\x1b[34^", Key::F10 },
    { "\x1b[34~", Key::F10 },
    { "\x1b[3;2~", Key::DELETE },
    { "\x1b[3;3~", Key::DELETE },
    { "\x1b[3;4~", Key::DELETE },
    { "\x1b[3;5~", Key::DELETE },
    { "\x1b[3;6~", Key::DELETE },
    { "\x1b[3;7~", Key::DELETE },
    { "\x1b[3;8~", Key::DELETE },
    { "\x1b[3@", Key::DELETE },
    { "\x1b[3^", Key::DELETE },
    { "\x1b[3~", Key::DELETE },
    { "\x1b[5$", Key::PAGE_UP },
    { "\x1b[5;2~", Key::PAGE_UP },
    { "\x1b[5;3~", Key::PAGE_UP },
    { "\x1b[5;4~", Key::PAGE_UP },
    { "\x1b[5;5~", Key::PAGE_UP },
    { "\x1b[5;6~", Key::PAGE_UP },
    { "\x1b[5;7~", Key::PAGE_UP },
    { "\x1b[5;8~", Key::PAGE_UP },
    { "\x1b[5@", Key::PAGE_UP },
    { "\x1b[5^", Key::PAGE_UP },
    { "\x1b[5~", Key::PAGE_UP },
    { "\x1b[6$", Key::PAGE_DOWN },
    { "\x1b[6;2~", Key::PAGE_DOWN },
    { "\x1b[6;3~", Key::PAGE_DOWN },
    { "\x1b[6;4~", Key::PAGE_DOWN },
    { "\x1b[6;5~", Key::PAGE_DOWN },
    { "\x1b[6;6~", Key::PAGE_DOWN },
    { "\x1b[6;7~", Key::PAGE_DOWN },
    { "\x1b[6;8~", Key::PAGE_DOWN },
    { "\x1b[6@", Key::PAGE_DOWN },
    { "\x1b[6^", Key::PAGE_DOWN },
    { "\x1b[6~", Key::PAGE_DOWN },
    { "\x1b[7$", Key::HOME },
    { "\x1b[7@", Key::HOME },
    { "\x1b[7^", Key::HOME },
    { "\x1b[7~", Key::HOME },
    { "\x1b[8$", Key::END },
    { "\x1b[8@", Key::END },
    { "\x1b[8^", Key::END },
    { "\x1b[8~", Key::END },
    { "\x1b[a", Key::ARROW_UP },
    { "\x1b[b", Key::ARROW_DOWN },
    { "\x1b[c", Key::ARROW_RIGHT },
    { "\x1b[d", Key::ARROW_LEFT },
    { "\x1bOM", Key::ENTER },
    { "\x1bOa", Key::ARROW_UP },
    { "\x1bOb", Key::ARROW_DOWN },
    { "\x1bOc", Key::ARROW_RIGHT },
    { "\x1bOd", Key::ARROW_LEFT },
    { "\x1bOl", '+' },
    { "\x1bOn", '.' },
    { "\x1bOp", Key::KP0 },
    { "\x1bOq", Key::KP1 },
    { "\x1bOr", Key::KP2 },
    { "\x1bOs", Key::KP3 },
    { "\x1bOt", Key::KP4 },
    { "\x1bOu", Key::KP5 },
    { "\x1bOv", Key::KP6 },
    { "\x1bOw", Key::KP7 },
    { "\x1bOx", Key::KP8 },
    { "\x1bOy", Key::KP9 },
};


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
int Terminal::get_key()
{
    int the_key = getch();

    if (the_key == 27)
    {
        std::string key_str = "\x1b";
        nodelay(stdscr, true);
        do
        {
            the_key = getch();
            if (the_key > 0 && the_key != Key::ESCAPE) key_str += std::string(1, static_cast<char>(the_key));
        } while (the_key > 0);
        nodelay(stdscr, false);
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
        case 1: case 2: return the_key;
        case 3: case 4: return Key::CLOSE;
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
        default: return Key::UNKNOWN;
    }
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
