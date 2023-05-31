# src/tune

The tune source folder is for headers which contain only definitions (usually `constexpr` or `enum`), which are used in many other places. These headers can be
tweaked to make sweeping changes to large parts of the game code at once.

* **area-generation.hpp** -- Hard-coded values which are used during area/dungeon generation, and can be tweaked to adjust the outcome.

* **ascii-symbols.hpp** -- Definitions for which ASCII symbols are used for which things in the game, allowing for easily changing them en masse if needed.

* **combat.hpp** -- Predefined variables which can be tweaked to rebalance aspects of combat.

* **error-handling.hpp** -- Tune values for adjusting error-handling in the Guru Meditation subsystem.

* **fov-lighting.hpp** -- Tune values for field-of-view, line-of-sight and static/dynamic lighting.

* **message-log.hpp** -- Tune values specific to the scrolling message log at the bottom of the screen.

* **nearby-bar.hpp** -- Tune values for the 'nearby' sidebar, which shows players what the various visible ASCII symbols represent.

* **resting.hpp** -- Preset values involving resting, and noises that are loud enough to wake the player.

* **timing.hpp** - All definitions for timing in the game (i.e. how long actions take to perform).
