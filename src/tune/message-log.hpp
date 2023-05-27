// tune/message-log.hpp -- Tune values specific to the scrolling message log at the bottom of the screen.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_MESSAGE_LOG_HPP_
#define TUNE_MESSAGE_LOG_HPP_

namespace invictus
{

constexpr int   AWAKEN_CHANCE_ALWAYS_SHOW_BUT_NEVER_WAKE = 255; // Don't change this.
constexpr int   MESSAGE_LOG_FADE_TIMER =        100;    // The amount of time between messages before a message is considered 'old' and de-bolded.
constexpr int   MESSAGE_LOG_HEIGHT =            10;     // The height of the message log window.
constexpr int   MESSAGE_LOG_OUTPUT_BUFFER_MAX = 10;     // The maximum number of lines stored in the output buffer.

}       // namespace invictus
#endif  // TUNE_MESSAGE_LOG_HPP_
