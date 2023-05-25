// util/timer.hpp -- A simple timer class for handling common in-game timing functionality.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_TIMER_HPP_
#define UTIL_TIMER_HPP_

#include <chrono>


namespace invictus
{

class Timer
{
public:
                    Timer(unsigned int timer_length = 0);   // Constructor, sets up initial time stamp.
    unsigned int    elapsed() const;    // Checks how many milliseconds have passed since this timer began.
    bool            expired() const;    // Checks if this timer has reached its expiry length.
    void            reset(unsigned int timer_length = 0);   // Resets the timer to the time now. Exactly the same functionality as the constructor.

private:
    std::chrono::time_point<std::chrono::steady_clock>  start_time_;    // The time that this Timer started counting.
    unsigned int    timer_length_;  // The length of the timer before it expires, if any.
};

}       // namespace invictus
#endif  // UTIL_TIMER_HPP_
