# src/core

The core source folder contains the most core and central elements of the game code, which are used almost everywhere -- this includes main program entry,
the main game loop, user preferences, error-handling, etc.

* **core.cpp** - Main program entry, initialization and cleanup routines, along with links to the key subsystems of the game.

* **game-manager.cpp** - The GameManager class manages the currently-running game state, as well as handling save/load functions.

* **guru.cpp** - Guru Meditation error-handling and reporting system.

* **prefs.cpp** - User-defined preferences, which can be set in userdata/prefs.txt

* **save-load.cpp** - Handles saving and loading the game state to/from disk.

* **version.hpp** - The version number of the game.
