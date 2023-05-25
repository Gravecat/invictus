# src/terminal

The terminal source folder contains all middleware code dealing with NCurses/PDCurses, and anything Curses or terminal-related.

* **escape-code-index.cpp** - Contains a map of escape code sequences, mapped to the internal `Key` enum, to convert all known terminal escape code sequences into useable key codes. *(Technically this should be a part of terminal.cpp, but due to its bulk, it is included separately here.)*
* **terminal-shared-defs.hpp** - Definitions used by both `terminal.hpp` and `window.hpp`; in order to keep these source files separate, they both use this shared header file to declare enums and such used by both.
* **terminal.cpp** - The primary middleware layer between the hell of NCurses *(and the slightly lesser hell of PDCurses)* and the rest of the code.
* **window.cpp** - Another middleware layer, this time making it more convenient to manage Curses panels/windows without using the Curses API directly.
