# src/tune

The tune source folder is for headers which contain only definitions (usually `constexpr` or `enum`), which are used in many other places. These headers can be
tweaked to make sweeping changes to large parts of the game code at once.

* **area-generation.hpp** -- `constexpr` values that adjust parameters when procedurally generating areas and dungeons.

* **ascii-symbols.hpp** -- `constexpr char` values to define the ASCII symbols used in the game. This way, they can be changed or swapped out en masse with
relative ease in the future, if needed.

* **error-handling.hpp** -- Tune values for adjusting error-handling in the Guru Meditation subsystem.

* **fov-lighting.hpp** -- Tune values for field-of-view, line-of-sight and static/dynamic lighting.

* **message-log.hpp** -- Tune values specific to the scrolling message log at the bottom of the screen.

* **nearby-bar.hpp** -- Tune values for the 'nearby' sidebar, which shows players what the various visible ASCII symbols represent.

* **timing.hpp** - `constexpr` values for the time it takes to perform various actions in-game, or other fixed timing values.
