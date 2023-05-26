# src/misc

The misc folder contains those few odd source files that just... don't really have anywhere more suitable to live. They may later get moved to another subfolder,
or they may end up living here forever.

* **ascii-symbols.hpp** -- `constexpr char` values to define the ASCII symbols used in the game. This way, they can be changed or swapped out en masse with
relative ease in the future, if needed.

* **timing.hpp** - `constexpr` values for the time it takes to perform various actions in-game, or other fixed timing values.
