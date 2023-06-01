// ui/title.cpp -- The title screen, also handles main-menu options such as loading saved games.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/version.hpp"
#include "terminal/terminal.hpp"
#include "ui/title.hpp"
#include "ui/ui.hpp"
#include "util/filex.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Constructor.
TitleScreen::TitleScreen() : save_exists_(save_file_exists()), selected_(0) { }

// Renders the title screen.
void TitleScreen::render()
{
    auto terminal = core()->terminal();
    int midrow = terminal->get_midrow(), midcol = terminal->get_midcol();

    terminal->cls();

    int dragon_x = midcol - 25, dragon_y = midrow - 10;
    int copyright_y = midrow + 5;
    int menu_x = dragon_x + 30, menu_y = dragon_y + 5;
    int gpl_y = terminal->get_rows() - 2;
    int logo_x = menu_x - 2, logo_y = menu_y - 4;

    terminal->print("{g}.~{r}* {R}MORIOR INVICTUS {r}*{g}~.", logo_x, logo_y);
    std::string version_str = "{r}Ver. " + INVICTUS_VERSION_STRING;
    terminal->print(version_str, logo_x - (StrX::strlen_colour(version_str) / 2) + 11, logo_y + 1);

    terminal->print("{G}/\\/\\", dragon_x + 15, dragon_y);
    terminal->print("{G}(   {M}O{G}_oo", dragon_x + 14, dragon_y + 1);
    terminal->print("{G}/\\  /\\   / (__{W},,{G}|", dragon_x + 5, dragon_y + 2);
    terminal->print("{G}) /^\\) ^\\/ {Y}_)", dragon_x + 4, dragon_y + 3);
    terminal->print("{G})   /^\\/   {Y}_)", dragon_x + 4, dragon_y + 4);
    terminal->print("{G})      /  /{Y}_)    {W},,", dragon_x + 4, dragon_y + 5);
    terminal->print("{G})      | | {Y})_){G}\\ (  )", dragon_x + 4, dragon_y + 6);
    terminal->print("{G})   _  | | {Y})_) {G}\\/ /", dragon_x + 4, dragon_y + 7);
    terminal->print("{G}/\\ )/\\/ \\ | | {Y})_){G}\\__/", dragon_x + 1, dragon_y + 8);
    terminal->print("{G}<  >      )({W},,{G}){Y})__)", dragon_x, dragon_y + 9);
    terminal->print("{G}||      /   \\{Y})___){G}\\", dragon_x + 1, dragon_y + 10);
    terminal->print("{G}| \\____(     {Y})___) {G})___", dragon_x + 1, dragon_y + 11);
    terminal->print("{G}\\______(______{W};;; {G}__{W};;;", dragon_x + 2, dragon_y + 12);

    terminal->print("Copyright (c) 2023 Raine \"Gravecat\" Simmons", midcol - 21, copyright_y, Colour::BLUE_BOLD);
    terminal->print("and the Morior Invictus project contributors.", midcol - 22, copyright_y + 1, Colour::BLUE_BOLD);
    terminal->print("github.com/Gravecat/invictus", midcol - 14, copyright_y + 3, Colour::BLUE_BOLD);

    terminal->print("Morior Invictus is free and open-source, licensed under the GNU AGPL v3.", midcol - 36, gpl_y, Colour::BLUE);

    terminal->print("  New Game  ", menu_x + 3, menu_y, Colour::YELLOW_BOLD, selected_ == 0 ? PRINT_FLAG_REVERSE : 0);
    terminal->print("  Load Game  ", menu_x + 3, menu_y + 2, (save_exists_ ? Colour::YELLOW_BOLD : Colour::BLACK_BOLD), selected_ == 1 ?
        PRINT_FLAG_REVERSE : 0);
    terminal->print("  Hall of Legends  ", menu_x, menu_y + 4, Colour::BLACK_BOLD, selected_ == 2 ? PRINT_FLAG_REVERSE : 0);
    terminal->print("  Quit Game  ", menu_x + 3, menu_y + 6, Colour::YELLOW_BOLD, selected_ == 3 ? PRINT_FLAG_REVERSE : 0);

    terminal->flip();
}

// Displays the title screen.
void TitleScreen::title_screen()
{
    auto terminal = core()->terminal();
    auto game = core()->game();
    int key = 0;
    bool redraw = true;

    while (true)
    {
        if (redraw)
        {
            render();
            redraw = false;
        }

        key = terminal->get_key();
        
        switch(key)
        {
            case Key::RESIZE:
                game->ui()->window_resized();
                redraw = true;
                break;
            case 'k': case Key::ARROW_UP: case Key::KP8:
                if (selected_ > 0)
                {
                    selected_--;
                    redraw = true;
                }
                break;
            case 'j': case Key::ARROW_DOWN: case Key::KP2:
                if (selected_ < 3)
                {
                    selected_++;
                    redraw = true;
                }
                break;
            case Key::ENTER: case ' ':
                switch(selected_)
                {
                    case 0:
                        game->set_game_state(GameState::NEW_GAME);
                        terminal->cls();
                        return;
                    case 1:
                        if (save_exists_)
                        {
                            game->set_game_state(GameState::LOAD_GAME);
                            terminal->cls();
                            return;
                        }
                        break;
                    case 2: break;  // Not implemented yet.
                    case 3:
                        core()->cleanup();
                        exit(EXIT_SUCCESS);
                }
                break;
        }
    }
}

// Check if a saved game exists.
bool TitleScreen::save_file_exists() const
{ return FileX::file_exists(core()->game()->save_folder() + "/game.dat"); }

}   // namespace invictus
