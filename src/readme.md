# src

This folder contains all of the source code for the project, including third-party libraries.

* **3rdparty** - The 3rdparty source folder is for third-party code that Morior Invictus relies upon. Nothing in this folder was written by me.

* **area** - The area source folder contains the Area class (which defines dungeons, towns, and other regions in the game that the player can explore), as well
as the Tile class (Tiles are the component blocks that make up an Area), and anything directly related to these classes.

* **core** - The core source folder contains the most core and central elements of the game code, which are used almost everywhere -- this includes main program
entry, the main game loop, user preferences, error-handling, etc.

* **dev** - The dev source folder contains testing and debugging code, some of which may be accessible to the user via launch parameters or in-game debug
commands.

* **entity** - The entity source folder contains the Entity class, which defines all objects that inhabit the game world -- monsters, items, chests, and so on.
Entity has multiple derived classes, which represent more specialized forms.

* **factory** - The factory source folder is where the static game data resides -- it contains factory classes, which generate preset classes for tiles, items,
monsters, etc.

* **terminal** - The terminal source folder contains all middleware code dealing with NCurses/PDCurses, and anything Curses or terminal-related.

* **tune** - The tune source folder contains static definitions that are used frequently in other parts of the code, and can be adjusted to tune aspects of the
game.

* **ui** - The ui source folder contains code dealing with the UI elements of the game (the dungeon viewport, the message log, the sidebar, other overlapping
windows), along with menus, popup boxes, and other UI-like things.

* **util** - The util source folder contains largely generic, standalone utility functions which are used elsewhere in the code, but are not tied directly to
one particular system.
