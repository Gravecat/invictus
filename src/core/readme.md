# src/core

The core source folder contains the most core and central elements of the game code, which are used almost everywhere -- this includes main program entry,
the main game loop, user preferences, error-handling, etc.

* **core.cpp** - Main program entry, initialization and cleanup routines, as well as pointers to various subsystems.
* **game-manager.cpp** - Handles the main game loop and the overall high-level state of the game (`core.cpp` handles the low-level stuff).
* **guru.cpp** - Guru Meditation, handles logging and exception/error/signal handling.
* **prefs.cpp** - User-defined preferences (can be edited by the user in the build/bin/userdata/prefs.txt file).