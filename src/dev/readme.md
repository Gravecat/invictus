# src/dev

The dev source folder contains testing and debugging code, some of which may be accessible to the user via launch parameters or in-game debug commands.

* **acs-display.cpp** -- Accessible by launching the game with the `-acs-display` parameter. Displays the ACS glyphs, for determining what terminals can support
which glyphs.

* **keycode-check.cpp** -- Accessible by launching the game with the `-keycode-check` parameter. Debug/testing code to check user inputs from Curses, and report
unknown keycodes or escape sequences.
