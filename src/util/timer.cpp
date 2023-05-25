// util/timer.cpp -- A simple timer class for handling common in-game timing functionality.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "util/timer.hpp"


namespace invictus
{

// Constructor, sets up initial time stamp.
Timer::Timer(unsigned int timer_length) : start_time_(std::chrono::steady_clock::now()), timer_length_(timer_length) { }

 // Checks how many milliseconds have passed since this timer began.
unsigned int Timer::elapsed() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time_).count();
}

// Checks if this timer has reached its expiry length.
bool Timer::expired() const
{
    if (timer_length_ && elapsed() >= timer_length_) return true;
    else return false;
}

// Resets the timer to the time now. Exactly the same functionality as the constructor.
void Timer::reset(unsigned int timer_length)
{
    start_time_ = std::chrono::steady_clock::now();
    timer_length_ = timer_length;
}


}   // namespace invictus
