#include "core/terminal.hpp"

#include <curses.h>

int main(int, char**)
{
    auto term = new Terminal();
    uint16_t key;
    int x, y;
    do
    {
        getmaxyx(stdscr, y, x);
        term->cls();
        term->print("{G}" + std::to_string(x) + "{w},{R}" + std::to_string(y), 1, 1);
        term->flip();
        key = term->get_key();
    } while (key != Key::CLOSE);
    delete term;
    return 0;
}
