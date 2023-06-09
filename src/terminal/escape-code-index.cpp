// terminal/escape-code-index.cpp -- Lookup table to convert terminal escape codes into keys.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "terminal/terminal.hpp"


namespace invictus
{

std::map<std::string, int> Terminal::escape_code_index_ = {
    { "\x1b", Key::ESCAPE }, { "\x1b[11^", Key::F1 }, { "\x1b[11~", Key::F1 }, { "\x1b[12^", Key::F2 }, { "\x1b[12~", Key::F2 }, { "\x1b[13^", Key::F3 },
    { "\x1b[13~", Key::F3 }, { "\x1b[14^", Key::F4 }, { "\x1b[14~", Key::F4 }, { "\x1b[15;2~", Key::F5 }, { "\x1b[15;3~", Key::F5 }, { "\x1b[15;4~", Key::F5 },
    { "\x1b[15;5~", Key::F5 }, { "\x1b[15;6~", Key::F5 }, { "\x1b[15;7~", Key::F5 }, { "\x1b[15;8~", Key::F5 }, { "\x1b[15^", Key::F5 },
    { "\x1b[15~", Key::F5 }, { "\x1b[17;2~", Key::F6 }, { "\x1b[17;3~", Key::F6 }, { "\x1b[17;4~", Key::F6 }, { "\x1b[17;5~", Key::F6 },
    { "\x1b[17;6~", Key::F6 }, { "\x1b[17;7~", Key::F6 }, { "\x1b[17;8~", Key::F6 }, { "\x1b[17^", Key::F6 }, { "\x1b[17~", Key::F6 },
    { "\x1b[18;2~", Key::F7 }, { "\x1b[18;3~", Key::F7 }, { "\x1b[18;4~", Key::F7 }, { "\x1b[18;5~", Key::F7 }, { "\x1b[18;6~", Key::F7 },
    { "\x1b[18;7~", Key::F7 }, { "\x1b[18;8~", Key::F7 }, { "\x1b[18^", Key::F7 }, { "\x1b[18~", Key::F7 }, { "\x1b[19;2~", Key::F8 },
    { "\x1b[19;3~", Key::F8 }, { "\x1b[19;4~", Key::F8 }, { "\x1b[19;5~", Key::F8 }, { "\x1b[19;6~", Key::F8 }, { "\x1b[19;7~", Key::F8 },
    { "\x1b[19;8~", Key::F8 }, { "\x1b[19^", Key::F8 }, { "\x1b[19~", Key::F8 }, { "\x1b[1;2A", Key::ARROW_UP }, { "\x1b[1;2B", Key::ARROW_DOWN },
    { "\x1b[1;2C", Key::ARROW_RIGHT }, { "\x1b[1;2D", Key::ARROW_LEFT }, { "\x1b[1;2F", Key::END }, { "\x1b[1;2H", Key::HOME }, { "\x1b[1;2P", Key::F1 },
    { "\x1b[1;2Q", Key::F2 }, { "\x1b[1;2R", Key::F3 }, { "\x1b[1;2S", Key::F4 }, { "\x1b[1;3A", Key::ARROW_UP }, { "\x1b[1;3B", Key::ARROW_DOWN },
    { "\x1b[1;3C", Key::ARROW_RIGHT }, { "\x1b[1;3D", Key::ARROW_LEFT }, { "\x1b[1;3F", Key::END }, { "\x1b[1;3H", Key::HOME }, { "\x1b[1;3P", Key::F1 },
    { "\x1b[1;3Q", Key::F2 }, { "\x1b[1;3R", Key::F3 }, { "\x1b[1;3S", Key::F4 }, { "\x1b[1;4A", Key::ARROW_UP }, { "\x1b[1;4B", Key::ARROW_DOWN },
    { "\x1b[1;4C", Key::ARROW_RIGHT }, { "\x1b[1;4D", Key::ARROW_LEFT }, { "\x1b[1;4F", Key::END }, { "\x1b[1;4H", Key::HOME }, { "\x1b[1;4P", Key::F1 },
    { "\x1b[1;4Q", Key::F2 }, { "\x1b[1;4R", Key::F3 }, { "\x1b[1;4S", Key::F4 }, { "\x1b[1;5A", Key::ARROW_UP }, { "\x1b[1;5B", Key::ARROW_DOWN },
    { "\x1b[1;5C", Key::ARROW_RIGHT }, { "\x1b[1;5D", Key::ARROW_LEFT }, { "\x1b[1;5F", Key::END }, { "\x1b[1;5H", Key::HOME }, { "\x1b[1;5P", Key::F1 },
    { "\x1b[1;5Q", Key::F2 }, { "\x1b[1;5R", Key::F3 }, { "\x1b[1;5S", Key::F4 }, { "\x1b[1;6A", Key::ARROW_UP }, { "\x1b[1;6B", Key::ARROW_DOWN },
    { "\x1b[1;6C", Key::ARROW_RIGHT }, { "\x1b[1;6D", Key::ARROW_LEFT }, { "\x1b[1;6F", Key::END }, { "\x1b[1;6H", Key::HOME }, { "\x1b[1;6P", Key::F1 },
    { "\x1b[1;6Q", Key::F2 }, { "\x1b[1;6R", Key::F3 }, { "\x1b[1;6S", Key::F4 }, { "\x1b[1;7A", Key::ARROW_UP }, { "\x1b[1;7B", Key::ARROW_DOWN },
    { "\x1b[1;7C", Key::ARROW_RIGHT }, { "\x1b[1;7D", Key::ARROW_LEFT }, { "\x1b[1;7F", Key::END }, { "\x1b[1;7H", Key::HOME }, { "\x1b[1;7P", Key::F1 },
    { "\x1b[1;7Q", Key::F2 }, { "\x1b[1;7R", Key::F3 }, { "\x1b[1;7S", Key::F4 }, { "\x1b[1;8A", Key::ARROW_UP }, { "\x1b[1;8B", Key::ARROW_DOWN },
    { "\x1b[1;8C", Key::ARROW_RIGHT }, { "\x1b[1;8D", Key::ARROW_LEFT }, { "\x1b[1;8F", Key::END }, { "\x1b[1;8H", Key::HOME }, { "\x1b[1;8P", Key::F1 },
    { "\x1b[1;8Q", Key::F2 }, { "\x1b[1;8R", Key::F3 }, { "\x1b[1;8S", Key::F4 }, { "\x1b[2$", Key::INSERT }, { "\x1b[20;2~", Key::F9 },
    { "\x1b[20;3~", Key::F9 }, { "\x1b[20;4~", Key::F9 }, { "\x1b[20;5~", Key::F9 }, { "\x1b[20;6~", Key::F9 }, { "\x1b[20;7~", Key::F9 },
    { "\x1b[20;8~", Key::F9 }, { "\x1b[20^", Key::F9 }, { "\x1b[20~", Key::F9 }, { "\x1b[21;2~", Key::F10 }, { "\x1b[21;3~", Key::F10 },
    { "\x1b[21;4~", Key::F10 }, { "\x1b[21;5~", Key::F10 }, { "\x1b[21;6~", Key::F10 }, { "\x1b[21;7~", Key::F10 }, { "\x1b[21;8~", Key::F10 },
    { "\x1b[21^", Key::F10 }, { "\x1b[21~", Key::F10 }, { "\x1b[23$", Key::F11 }, { "\x1b[23;2~", Key::F11 }, { "\x1b[23;3~", Key::F11 },
    { "\x1b[23;4~", Key::F11 }, { "\x1b[23;5~", Key::F11 }, { "\x1b[23;6~", Key::F11 }, { "\x1b[23;7~", Key::F11 }, { "\x1b[23;8~", Key::F11 },
    { "\x1b[23@", Key::F11 }, { "\x1b[23^", Key::F1 }, { "\x1b[23^", Key::F11 }, { "\x1b[23~", Key::F1 }, { "\x1b[23~", Key::F11 }, { "\x1b[24$", Key::F12 },
    { "\x1b[24;2~", Key::F12 }, { "\x1b[24;3~", Key::F12 }, { "\x1b[24;4~", Key::F12 }, { "\x1b[24;5~", Key::F12 }, { "\x1b[24;6~", Key::F12 },
    { "\x1b[24;7~", Key::F12 }, { "\x1b[24;8~", Key::F12 }, { "\x1b[24@", Key::F12 }, { "\x1b[24^", Key::F12 }, { "\x1b[24^", Key::F2 },
    { "\x1b[24~", Key::F12 }, { "\x1b[24~", Key::F2 }, { "\x1b[25^", Key::F3 }, { "\x1b[25~", Key::F3 }, { "\x1b[26^", Key::F4 }, { "\x1b[26~", Key::F4 },
    { "\x1b[28^", Key::F5 }, { "\x1b[28~", Key::F5 }, { "\x1b[29^", Key::F6 }, { "\x1b[29~", Key::F6 }, { "\x1b[2;2~", Key::INSERT },
    { "\x1b[2;3~", Key::INSERT }, { "\x1b[2;4~", Key::INSERT }, { "\x1b[2;5~", Key::INSERT }, { "\x1b[2;6~", Key::INSERT }, { "\x1b[2;7~", Key::INSERT },
    { "\x1b[2;8~", Key::INSERT }, { "\x1b[2@", Key::INSERT }, { "\x1b[2^", Key::INSERT }, { "\x1b[2~", Key::INSERT }, { "\x1b[3$", Key::DELETE },
    { "\x1b[31^", Key::F7 }, { "\x1b[31~", Key::F7 }, { "\x1b[32^", Key::F8 }, { "\x1b[32~", Key::F8 }, { "\x1b[33^", Key::F9 }, { "\x1b[33~", Key::F9 },
    { "\x1b[34^", Key::F10 }, { "\x1b[34~", Key::F10 }, { "\x1b[3;2~", Key::DELETE }, { "\x1b[3;3~", Key::DELETE }, { "\x1b[3;4~", Key::DELETE },
    { "\x1b[3;5~", Key::DELETE }, { "\x1b[3;6~", Key::DELETE }, { "\x1b[3;7~", Key::DELETE }, { "\x1b[3;8~", Key::DELETE }, { "\x1b[3@", Key::DELETE },
    { "\x1b[3^", Key::DELETE }, { "\x1b[3~", Key::DELETE }, { "\x1b[5$", Key::PAGE_UP }, { "\x1b[5;2~", Key::PAGE_UP }, { "\x1b[5;3~", Key::PAGE_UP },
    { "\x1b[5;4~", Key::PAGE_UP }, { "\x1b[5;5~", Key::PAGE_UP }, { "\x1b[5;6~", Key::PAGE_UP }, { "\x1b[5;7~", Key::PAGE_UP }, { "\x1b[5;8~", Key::PAGE_UP },
    { "\x1b[5@", Key::PAGE_UP }, { "\x1b[5^", Key::PAGE_UP }, { "\x1b[5~", Key::PAGE_UP }, { "\x1b[6$", Key::PAGE_DOWN }, { "\x1b[6;2~", Key::PAGE_DOWN },
    { "\x1b[6;3~", Key::PAGE_DOWN }, { "\x1b[6;4~", Key::PAGE_DOWN }, { "\x1b[6;5~", Key::PAGE_DOWN }, { "\x1b[6;6~", Key::PAGE_DOWN },
    { "\x1b[6;7~", Key::PAGE_DOWN }, { "\x1b[6;8~", Key::PAGE_DOWN }, { "\x1b[6@", Key::PAGE_DOWN }, { "\x1b[6^", Key::PAGE_DOWN },
    { "\x1b[6~", Key::PAGE_DOWN }, { "\x1b[7$", Key::HOME }, { "\x1b[7@", Key::HOME }, { "\x1b[7^", Key::HOME }, { "\x1b[7~", Key::HOME },
    { "\x1b[8$", Key::END }, { "\x1b[8@", Key::END }, { "\x1b[8^", Key::END }, { "\x1b[8~", Key::END }, { "\x1b[a", Key::ARROW_UP },
    { "\x1b[b", Key::ARROW_DOWN }, { "\x1b[c", Key::ARROW_RIGHT }, { "\x1b[d", Key::ARROW_LEFT }, { "\x1bOM", Key::ENTER }, { "\x1bOa", Key::ARROW_UP },
    { "\x1bOb", Key::ARROW_DOWN }, { "\x1bOc", Key::ARROW_RIGHT }, { "\x1bOd", Key::ARROW_LEFT }, { "\x1bOl", '+' }, { "\x1bOn", '.' }, { "\x1bOp", Key::KP0 },
    { "\x1bOq", Key::KP1 }, { "\x1bOr", Key::KP2 }, { "\x1bOs", Key::KP3 }, { "\x1bOt", Key::KP4 }, { "\x1bOu", Key::KP5 }, { "\x1bOv", Key::KP6 },
    { "\x1bOw", Key::KP7 }, { "\x1bOx", Key::KP8 },  { "\x1bOy", Key::KP9 }, { "\x1b[1~", Key::HOME }, { "\x1b[4~", Key::END }
};

}   // namespace invictus
