// death.cpp -- Death comes for us all eventually.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "ui/death.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// The skull symbol to render on the game-over screen.
uint8_t Death::skull_pattern[4] = { 0x70, 0xFA, 0xED, 0xFB };


// The player has just died.
void Death::die()
{
    GameState game_state = core()->game()->game_state();
    if (game_state == GameState::DUNGEON_DEAD || game_state == GameState::DEAD) return;
    core()->message("{m}You have died without honour! {r}Press the space bar to continue...");
    core()->game()->set_game_state(GameState::DUNGEON_DEAD);
}

// Renders the game over screen.
void Death::render_death_screen()
{
    auto terminal = core()->terminal();
    core()->game()->ui()->dungeon_mode_ui(false);
    core()->game()->set_game_state(GameState::DEAD);
    bool redraw = true;
    int key = 0;

    while(true)
    {
        const int midcol = terminal->get_midcol(), midrow = terminal->get_midrow();

        if (redraw)
        {
            redraw = false;
            terminal->cls();

            for (int x = 0; x < 7; x++)
            {
                const int ax = midcol - 7 + (x * 2);
                uint8_t line_code = skull_pattern[x < 4 ? x : 6 - x ];
                for (int y = 0; y < 8; y++)
                {
                    const int ay = midrow - 5 + y;
                    if (line_code & (1 << (7 - y)))
                    {
                        terminal->put(' ', ax, ay, Colour::RED, PRINT_FLAG_REVERSE);
                        terminal->put(' ', ax + 1, ay, Colour::RED, PRINT_FLAG_REVERSE);
                    }
                }
            }
            terminal->print("YOU HAVE DIED... YOUR ADVENTURE HAS COME TO AN END", midcol - 25, midrow - 8, Colour::RED_BOLD);
            terminal->print("{g}.~{r}* {R}THANKS FOR PLAYING MORIOR INVICTUS {r}*{g}~.", midcol - 21, midrow + 5);
            terminal->print("PRESS THE SPACE BAR WHEN YOU ARE READY TO MOVE ON", midcol - 25, midrow + 7, Colour::RED_BOLD);
            terminal->flip();
        }

        key = terminal->get_key();
        if (key == Key::RESIZE) redraw = true;
        else if (key == ' ')
        {
            core()->cleanup();
            exit(EXIT_SUCCESS);
        }
    }
}

}   // namespace invictus
