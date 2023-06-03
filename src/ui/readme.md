# src/ui

The ui source folder contains code dealing with the UI elements of the game (the dungeon viewport, the message log, the sidebar, other overlapping windows),
along with menus, popup boxes, and other UI-like things.

* **bars.cpp** - UI code to render various forms of status bars.

* **menu.cpp** - Menu class definition, for a fairly generic scrollable menu of items.

* **msglog.cpp** - The message log, which displays important information to the player.

* **nearby.cpp** - The sidebar, showing NPCs and items nearby.

* **system-menu.cpp** - The main system menu, giving the player options such as saving or quitting the game.

* **title.cpp** - The title screen, also handles main-menu options such as loading saved games.

* **ui.cpp** - The UI overlay that displays additional information on top of the game's map, using other UI classes.

* **wiki.cpp** - Interactive documentation system, with links between pages.
* * **wiki-pages.cpp** - The in-game documentation, stored in a separate file from the wiki rendering code (wiki.cpp).
