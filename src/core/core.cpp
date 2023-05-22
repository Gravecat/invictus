#include "core/terminal.hpp"

#include <curses.h>

int main(int, char**)
{
    auto term = new Terminal();
    uint16_t key = 0;
    do
    {
        term->cls();
        term->print("{G}" + std::to_string(key), 1, 1);
        term->flip();
        key = term->get_key();
    } while (key != Key::CLOSE);
    delete term;
    return 0;
}
