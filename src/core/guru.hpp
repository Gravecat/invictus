// core/guru.hpp -- Guru Meditation error-handling and reporting system.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_GURU_HPP_
#define CORE_GURU_HPP_

#include <ctime>
#include <exception>
#include <fstream>
#include <string>


namespace invictus
{

class Guru
{
public:
    static constexpr int    GURU_INFO =     0;  // General logging information.
    static constexpr int    GURU_WARN =     1;  // Warnings, non-fatal stuff.
    static constexpr int    GURU_ERROR =    2;  // Serious errors. Shit is going down.
    static constexpr int    GURU_CRITICAL = 3;  // Critical system failure.

            Guru(std::string log_filename = "");                // Opens the output log for messages.
            ~Guru();                                            // Closes the Guru log file.
    void    console_ready(bool is_ready = true);                // Tells Guru that we're ready to render Guru error messages on-screen.
    void    halt(std::string error, int a = 0, int b = 0);      // Stops the game and displays an error messge.
    void    halt(std::exception &e);                            // As above, but with an exception instead of a string.
    void    hook_signals();                                     // Tells Guru to hook system failure signals.
    void    intercept_signal(int sig);                          // Catches a segfault or other fatal signal.
    bool    is_dead() const;                                    // Checks if the system has halted.
    void    log(std::string msg, int type = Guru::GURU_INFO);   // Logs a message in the system log file.
    void    nonfatal(std::string error, int type);              // Reports a non-fatal error, which will be logged but will not halt execution unless it cascades.

private:
    static constexpr int    CASCADE_THRESHOLD =         25;     // The amount cascade_count can reach within CASCADE_TIMEOUT seconds before it triggers an abort screen.
    static constexpr int    CASCADE_TIMEOUT =           30;     // The number of seconds without an error to reset the cascade timer.
    static constexpr int    CASCADE_WEIGHT_CRITICAL =   20;     // The amount a critical type log entry will add to the cascade timer.
    static constexpr int    CASCADE_WEIGHT_ERROR =      5;      // The amount an error type log entry will add to the cascade timer.
    static constexpr int    CASCADE_WEIGHT_WARNING =    1;      // The amount a warning type log entry will add to the cascade timer.
    static const char       FILENAME_LOG[];                     // The default name of the log file. Another filename can be specified with open_syslog().
    static constexpr int    HALT_FLASH_SPEED =          1000;   // The time in milliseconds that the halt screen's border flashes.

    void    close_log();    // Closes the log. Only for internal use.

    int                 cascade_count_;     // Keeps track of rapidly-occurring, non-fatal error messages.
    bool                cascade_failure_;   // Is a cascade failure in progress?
    time_t              cascade_timer_;     // Timer to check the speed of non-halting Guru warnings, to prevent cascade locks.
    bool                console_ready_;     // Have we fully initialized the console yet?
    bool                dead_already_;      // Have we already died? Is this crash within the Guru subsystem?
    std::ofstream       syslog_;            // The system log file.
};

}       // namespace invictus
#endif  // CORE_GURU_HPP_
