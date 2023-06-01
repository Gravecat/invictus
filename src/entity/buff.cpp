// entity/buff.cpp -- Buff/debuff system, allowing temporary effects to be placed on Mobiles.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "entity/buff.hpp"


namespace invictus
{

// Constructor.
Buff::Buff(BuffType type, int power, int time_left) : power_(power), time_left_(time_left), type_(type) { }

// Checks if this Buff has expired yet.
bool Buff::expired() const { return time_left_ <= 0; }

// Returns the power of this Buff.
int Buff::get_power() const { return power_; }

// Returns the time left on this Buff, in tick10() ticks.
int Buff::get_time_left() const { return time_left_; }

// Returns the type of this Buff.
BuffType Buff::get_type() const { return type_; }

// Sets the time left on this Buff.
void Buff::set_time(int time) { time_left_ = time;  }

// Sets the power of this Buff.
void Buff::set_power(int power) { power_ = power; }

// Ticks this Buff, reducing its time left.
void Buff::tick() { time_left_--; }

}   // namespace invictus
