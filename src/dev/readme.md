# src/dev

The dev source folder contains testing and debugging code, some of which may be accessible to the user via launch parameters or in-game debug commands.

* **acs-display.cpp** -- Accessible by launching the game with the `-acs-display` parameter, displays the Curses ACS VT100/teletype/SysV glyphs, to determine which display correctly on the current terminal.
* **keycode-check.cpp** -- Accessible by launching the game with the `-keycode-check` parameter, displays keycodes or escape sequences returned by Curses' `wgetch()` function, useful for checking if certain keys are not correctly supported yet on certain terminals.
