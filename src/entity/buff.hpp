// entity/buff.hpp -- Buff/debuff system, allowing temporary effects to be placed on Mobiles.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_BUFF_HPP_
#define ENTITY_BUFF_HPP_

#include <cstdint>


namespace invictus
{

enum class BuffType : uint16_t { NONE, INJURED };


class Buff
{
public:
                Buff(BuffType type, int power, int time_left);  // Constructor.
    bool        expired() const;        // Checks if this Buff has expired yet.
    int         get_power() const;      // Returns the power of this Buff.
    int         get_time_left() const;  // Returns the time left on this Buff, in tick10() ticks.
    BuffType    get_type() const;       // Returns the type of this Buff.
    void        tick();     // Ticks this Buff, reducing its time left.

private:
    int         power_;
    int         time_left_;
    BuffType    type_;

friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_BUFF_HPP_
