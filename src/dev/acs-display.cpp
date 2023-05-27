// dev/acs-display.cpp -- Accessible by launching the game with the `-acs-display` parameter.
// Displays the ACS glyphs, for determining what terminals can support which glyphs.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "dev/acs-display.hpp"
#include "terminal/terminal.hpp"


namespace invictus
{

// Displays the ACS glyphs.
void DevACSDisplay::display_test()
{
    auto terminal = core()->terminal();
    const int glyph_x = 30;
    terminal->print("ACS glyph rendering test!", 0, 0, Colour::WHITE_BOLD);

    terminal->print("VT100 box characters:", 0, 2, Colour::WHITE_BOLD);
    terminal->print("(ACS flag: 1)", glyph_x + 24, 2, Colour::WHITE);
    terminal->put(Glyph::ULCORNER, glyph_x, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::LLCORNER, glyph_x + 2, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::URCORNER, glyph_x + 4, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::LRCORNER, glyph_x + 6, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::RTEE, glyph_x + 8, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::LTEE, glyph_x + 10, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::BTEE, glyph_x + 12, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::TTEE, glyph_x + 14, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::HLINE, glyph_x + 16, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::VLINE, glyph_x + 18, 2, Colour::GREEN_BOLD);
    terminal->put(Glyph::PLUS, glyph_x + 20, 2, Colour::GREEN_BOLD);

    terminal->print("VT100 other characters:", 0, 4, Colour::WHITE_BOLD);
    terminal->print("(ACS flag: 2)", glyph_x + 24, 4, Colour::WHITE);
    terminal->put(Glyph::DIAMOND, glyph_x, 4, Colour::GREEN_BOLD);
    terminal->put(Glyph::CKBOARD, glyph_x + 2, 4, Colour::YELLOW_BOLD);
    terminal->put(Glyph::DEGREE, glyph_x + 4, 4, Colour::GREEN_BOLD);
    terminal->put(Glyph::PLMINUS, glyph_x + 6, 4, Colour::GREEN_BOLD);
    terminal->put(Glyph::BULLET, glyph_x + 8 , 4, Colour::GREEN_BOLD);

    terminal->print("Teletype 5410v1 characters:", 0, 6, Colour::WHITE_BOLD);
    terminal->print("(ACS flag: 4)", glyph_x + 24, 6, Colour::WHITE);
    terminal->put(Glyph::LARROW, glyph_x, 6, Colour::YELLOW_BOLD);
    terminal->put(Glyph::RARROW, glyph_x + 2, 6, Colour::YELLOW_BOLD);
    terminal->put(Glyph::DARROW, glyph_x + 4, 6, Colour::YELLOW_BOLD);
    terminal->put(Glyph::UARROW, glyph_x + 6, 6, Colour::YELLOW_BOLD);
    terminal->put(Glyph::BOARD, glyph_x + 8, 6, Colour::YELLOW_BOLD);
    terminal->put(Glyph::LANTERN, glyph_x + 10, 6, Colour::RED_BOLD);
    terminal->put(Glyph::BLOCK, glyph_x + 12, 6, Colour::YELLOW_BOLD);

    terminal->print("Undocumented SysV characters:", 0, 8, Colour::WHITE_BOLD);
    terminal->print("(ACS flag: 8)", glyph_x + 24, 8, Colour::WHITE);
    terminal->put(Glyph::LEQUAL, glyph_x, 8, Colour::GREEN_BOLD);
    terminal->put(Glyph::GEQUAL, glyph_x + 2, 8, Colour::GREEN_BOLD);
    terminal->put(Glyph::PI, glyph_x + 4, 8, Colour::GREEN_BOLD);
    terminal->put(Glyph::NEQUAL, glyph_x + 6, 8, Colour::GREEN_BOLD);
    terminal->put(Glyph::STERLING, glyph_x + 8, 8, Colour::GREEN_BOLD);

    terminal->print("ACS S1, S3, S7, S9:", 0, 10, Colour::WHITE_BOLD);
    terminal->print("(ACS flag: 16)", glyph_x + 24, 10, Colour::WHITE);
    terminal->put(Glyph::S1, glyph_x, 10, Colour::RED_BOLD);
    terminal->put(Glyph::S3, glyph_x + 2, 10, Colour::RED_BOLD);
    terminal->put(Glyph::S7, glyph_x + 4, 10, Colour::RED_BOLD);
    terminal->put(Glyph::S9, glyph_x + 6, 10, Colour::RED_BOLD);

    terminal->flip();
    terminal->get_key();
}

}   // namespace invictus
