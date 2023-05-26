// tune/error-handling.hpp -- Tune values for adjusting error-handling in the Guru Meditation subsystem.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_ERROR_HANDLING_HPP_
#define TUNE_ERROR_HANDLING_HPP_

namespace invictus
{

constexpr int    ERROR_CASCADE_THRESHOLD =          25; // The amount cascade_count can reach within CASCADE_TIMEOUT seconds before it triggers an abort.
constexpr int    ERROR_CASCADE_TIMEOUT =            30; // The number of seconds without an error to reset the cascade timer.
constexpr int    ERROR_CASCADE_WEIGHT_CRITICAL =    20; // The amount a critical type log entry will add to the cascade timer.
constexpr int    ERROR_CASCADE_WEIGHT_ERROR =       5;  // The amount an error type log entry will add to the cascade timer.
constexpr int    ERROR_CASCADE_WEIGHT_WARNING =     1;  // The amount a warning type log entry will add to the cascade timer.

}       // nmamespace invictus
#endif  // TUNE_ERROR_HANDLING_HPP_
