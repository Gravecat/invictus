// terminal/terminal-shared-defs.hpp -- Definitions shared and used by both Terminal and Window classes.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TERMINAL_TERMINAL_SHARED_DEFS_HPP_
#define TERMINAL_TERMINAL_SHARED_DEFS_HPP_

#include <climits>
#include <cstdint>


#ifdef INVICTUS_TARGET_WINDOWS
typedef struct _win WINDOW;
#else
typedef struct _win_st WINDOW;
#endif
typedef struct panel PANEL;


namespace invictus
{

constexpr int   PRINT_FLAG_BOLD =       1;  // The specified string should be printed in bold.
constexpr int   PRINT_FLAG_REVERSE =    2;  // The string's colours should be inverted.
constexpr int   PRINT_FLAG_BLINK =      4;  // Blinking colour effect.

enum class Colour : uint8_t { NONE, BLACK, BLACK_BOLD, RED, RED_BOLD, GREEN, GREEN_BOLD, YELLOW, YELLOW_BOLD, BLUE, BLUE_BOLD, MAGENTA, MAGENTA_BOLD, CYAN,
    CYAN_BOLD, WHITE, WHITE_BOLD };

enum class Glyph : uint16_t { ULCORNER = 256, LLCORNER, URCORNER, LRCORNER, RTEE, LTEE, BTEE, TTEE, HLINE, VLINE, PLUS, S1, S9, DIAMOND, CKBOARD, DEGREE, PLMINUS,
    BULLET, LARROW, RARROW, DARROW, UARROW, BOARD, LANTERN, BLOCK, S3, S7, LEQUAL, GEQUAL, PI, NEQUAL, STERLING };

enum Key { BACKSPACE = 8, TAB = 9, ENTER = 10, CR = 13, ESCAPE = 27, CLOSE = 256, RESIZE, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, DELETE, INSERT, HOME, END,
    PAGE_UP, PAGE_DOWN, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, KP0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9, POUND, UNKNOWN_KEY,
    UNKNOWN_ESCAPE_SEQUENCE };


}       // namespace invictus
#endif  // TERMINAL_TERMINAL_SHARED_DEFS_HPP_
