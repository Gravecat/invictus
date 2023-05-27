# src/terminal

The terminal source folder contains all middleware code dealing with NCurses/PDCurses, and anything Curses or terminal-related.

* **escape-code-index.cpp** - Lookup table to convert terminal escape codes into keys.
*(Technically this should be a part of terminal.cpp, but due to its bulk, it is included separately here.)*

* **terminal-shared-defs.hpp** - Definitions shared and used by both Terminal and Window classes.

* **terminal.cpp** - Interface code to PDCurses/NCurses, to handle cross-platform compatability and generally take away the pain of using Curses' API.

* **window.cpp** - The Window class, part of the Terminal subsystem, allowing easier management of Curses windows and panels.
