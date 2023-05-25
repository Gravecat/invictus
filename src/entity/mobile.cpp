// entity/mobile.cpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/guru.hpp"
#include "entity/mobile.hpp"
#include "world/timing.hpp"


namespace invictus
{

// Constructor.
Mobile::Mobile() : Entity(), banked_ticks_(0)
{
    set_prop_f(EntityProp::SPEED, Timing::TIME_BASE_MOVEMENT);
}

// Adds or removes banked ticks to this Mobile.
void Mobile::add_banked_ticks(float amount)
{
    if (amount < 0)
    {
        core()->guru()->nonfatal("Attempt to add negative banked ticks to " + name() + ": " + std::to_string(amount) +
            " (did you mean to use spend_banked_ticks()?)", Guru::GURU_ERROR);
        clear_banked_ticks();
    }
    else banked_ticks_ += amount;
}

// Retrieves the amount of ticks banked by this Mobile.
float Mobile::banked_ticks() const { return banked_ticks_; }

// Erase all banked ticks on this Mobile.
void Mobile::clear_banked_ticks() { banked_ticks_ = 0; }

// Checks if this Mobile is dead.
bool Mobile::is_dead() const { return false; }

// Returns the amount of ticks needed for this Mobile to move one tile.
float Mobile::movement_speed() const { return get_prop_f(EntityProp::SPEED); }

// The exact opposite of add_banked_ticks().
void Mobile::spend_banked_ticks(float amount)
{
    if (amount < 0)
    {
        core()->guru()->nonfatal("Attempt to spend negative banked ticks on " + name() + ": " + std::to_string(amount) +
            " (did you mean to use add_banked_ticks()?)", Guru::GURU_ERROR);
        clear_banked_ticks();
    }
    else banked_ticks_ -= amount;
}

// Processes AI for this Mobile each turn.
void Mobile::tick(std::shared_ptr<Entity> self)
{
    if (is_dead()) return;

    if (type() == EntityType::PLAYER) return;

    add_banked_ticks(Timing::TICK_SPEED);
    auto self_mob = std::dynamic_pointer_cast<Mobile>(self);

    // There's nothing more to do.
    clear_banked_ticks();
}

// Process slower state-change events that happen less often, such as buffs/debuffs ticking.
void Mobile::tick10(std::shared_ptr<Entity>)
{
    if (is_dead()) return;
}

}   // namespace invictus
