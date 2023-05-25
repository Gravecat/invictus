// core/core.hpp -- Main program entry, initialization and cleanup routines, along with links to the key subsystems of the game.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_CORE_HPP_
#define CORE_CORE_HPP_

#include <memory>
#include <string>
#include <vector>

namespace invictus
{

class Guru;     // defined in core/guru.hpp
class Terminal; // defined in core/terminal.hpp


class Core
{
public:
            Core();     // Constructor, sets some default values.
            ~Core();    // Destructor, cleans up memory used and subsystems.
    void    cleanup();          // Attempts to gracefully clean up memory and subsystems.
    const std::shared_ptr<Guru>         guru() const;   // Returns a pointer to the Guru Meditation object.
    void    init(std::vector<std::string> parameters);  // Sets up the core game classes and data, and the terminal subsystem.
    const std::shared_ptr<Terminal>     terminal() const;   // Returns a pointer to the terminal emulator object.

private:
    std::shared_ptr<Guru>       guru_meditation_;   // The Guru Meditation error-handling system.
    std::shared_ptr<Terminal>   terminal_;  // The Terminal class, which handles low-level interaction with terminal emulation libraries.
};

const std::shared_ptr<Core> core(); // Allows external access to the main Core object.

}       // namespace invictus
#endif  // CORE_CORE_HPP_
