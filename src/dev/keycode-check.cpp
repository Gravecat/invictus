// dev/keycode-check.cpp -- Debug/testing code to check user inputs from Curses, and report unknown keycodes or escape sequences.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <string>

#include "core/core.hpp"
#include "dev/keycode-check.hpp"
#include "terminal/terminal.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Begins the keycode checking loop.
void DevKeycodeCheck::begin()
{
    auto terminal = core()->terminal();
    int key = 0, key_raw = 0;
    std::string key_str, escape_sequence, message;
    do
    {
        key = terminal->get_key();
        if (key <= 0 || key == Key::RESIZE) continue;
        key_str.clear();
        key_raw = terminal->last_key_raw();
        escape_sequence = terminal->last_escape_sequence();
        
        switch (key)
        {
            case ' ': key_str = "{G}Space"; break;
            case 8: key_str = "{G}Backspace {g}or {G}Ctrl-H"; break;
            case 9: key_str = "{G}Tab {g}or {G}Ctrl-I"; break;
            case 10: key_str = "{G}Enter{g}, {G}LF{g}, or {G}Ctrl-J"; break;
            case 13: key_str = "{G}CR {g}or {G}Ctrl-M"; break;
            case 27: key_str = "{G}Escape"; break;
            case Key::CLOSE:
                if (key_raw == 3) key_str = "{G}Ctrl-C";
                else if (key_raw == 0x130) key_str = "{G}Alt-F4";
                break;
            case Key::ARROW_UP: key_str = "{G}Arrow Up"; break;
            case Key::ARROW_DOWN: key_str = "{G}Arrow Down"; break;
            case Key::ARROW_LEFT: key_str = "{G}Arrow Left"; break;
            case Key::ARROW_RIGHT: key_str = "{G}Arrow Right"; break;
            case Key::DELETE: key_str = "{G}Delete"; break;
            case Key::INSERT: key_str = "{G}Insert"; break;
            case Key::HOME: key_str = "{G}Home"; break;
            case Key::END: key_str = "{G}End"; break;
            case Key::PAGE_UP: key_str = "{G}Page Up"; break;
            case Key::PAGE_DOWN: key_str = "{G}Page Down"; break;
            case Key::POUND: key_str = "{G}Pound"; break;
            case Key::NOT: key_str = "{G}Not"; break;
            case Key::UNKNOWN_KEY: key_str = "{R}Unknown Key"; break;
            case Key::UNKNOWN_ESCAPE_SEQUENCE: key_str = "{R}Unknown Escape Sequence"; break;
        }
        if (!key_str.size())
        {
            if (key >= 1 && key <= 26) key_str = "{G}Ctrl-" + std::string(1, key + 64);
            else if (key >= 27 && key <= 31) key_str = "{Y}Low-ASCII";
            else if (key >= 33  && key <= 127) key_str = "{G}" + std::string(1, key);
            else if (key >= Key::F1 && key <= Key::F12) key_str = "{G}F" + std::to_string(key - Key::F1 + 1);
            else if (key >= Key::KP0 && key <= Key::KP9) key_str = "{G}Keypad " + std::to_string(key - Key::KP0);
            if (!key_str.size()) key_str = "{M}Unrecognized Key {m}(this should be impossible! please report!)";
        }

        message = "{W}Detected ";
        if (escape_sequence.size() > 1) message += "escape sequence: {C}" + escape_sequence.substr(1) + " {W}(" + key_str + "{W})";
        else message += "key: {C}" + std::to_string(key_raw) + " {W}({C}0x" + StrX::str_toupper(StrX::itoh(key_raw, 1)) + "{W}) (" + key_str + "{W})";
        terminal->cls();
        terminal->print(message, 1, 1);
        terminal->flip();
    } while (key != Key::CLOSE);

    terminal->print("{W}Please press any key to exit.", 1, 3);
    terminal->get_key();
}

}   // namespace invictus
