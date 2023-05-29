// core/guru.hpp -- Guru Meditation error-handling and reporting system.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_GURU_HPP_
#define CORE_GURU_HPP_

#include <ctime>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>


namespace invictus
{

constexpr int    GURU_INFO =     0;  // General logging information.
constexpr int    GURU_WARN =     1;  // Warnings, non-fatal stuff.
constexpr int    GURU_ERROR =    2;  // Serious errors. Shit is going down.
constexpr int    GURU_CRITICAL = 3;  // Critical system failure.

class Guru
{
public:
            Guru(std::string log_filename = "");            // Opens the output log for messages.
            ~Guru();                                        // Destructor, calls cleanup code.
    void    check_stderr();                                 // Checks stderr for any updates, puts them in the log if any exist.
    void    cleanup();                                      // Closes the system log gracefully.
    void    console_ready(bool is_ready = true);            // Tells Guru that we're ready to render Guru error messages on-screen.
    void    halt(std::string error, int a = 0, int b = 0);  // Stops the game and displays an error messge.
    void    halt(std::exception &e);                        // As above, but with an exception instead of a string.
    void    hook_signals();                                 // Tells Guru to hook system failure signals.
    void    intercept_signal(int sig);                      // Catches a segfault or other fatal signal.
    bool    is_dead() const;                                // Checks if the system has halted.
    void    log(std::string msg, int type = GURU_INFO);     // Logs a message in the system log file.
    void    nonfatal(std::string error, int type);          // Reports a non-fatal error, which will be logged but won't halt execution unless it cascades.

private:
    int                 cascade_count_;     // Keeps track of rapidly-occurring, non-fatal error messages.
    bool                cascade_failure_;   // Is a cascade failure in progress?
    time_t              cascade_timer_;     // Timer to check the speed of non-halting Guru warnings, to prevent cascade locks.
    bool                cleanup_done_;      // Has the cleanup routine already run once?
    bool                console_ready_;     // Have we fully initialized the console yet?
    bool                dead_already_;      // Have we already died? Is this crash within the Guru subsystem?
    std::stringstream*  stderr_buffer_;     // Pointer to a stringstream buffer used to catch stderr messages.
    std::streambuf*     stderr_old_;        // The old stderr buffer.
    std::ofstream       syslog_;            // The system log file.
};

}       // namespace invictus
#endif  // CORE_GURU_HPP_
