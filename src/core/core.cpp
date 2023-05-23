#include "core/terminal.hpp"

#include <curses.h>

int main(int, char**)
{
    auto term = new Terminal();
    int key = 0;
    std::vector<int> keys;
    do
    {
        term->cls();
        for (unsigned int i = 0; i < keys.size(); i++)
            term->print("{G}" + std::to_string(keys.at(i)), 0, i);
        term->flip();
        key = term->get_key();
        if (key == ' ') keys.clear(); else keys.push_back(key);
    } while (key != Key::CLOSE);
    delete term;
    return 0;
}
