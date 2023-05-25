// entity/mobile.cpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/mobile.hpp"
#include "world/timing.hpp"


namespace invictus
{

// Constructor.
Mobile::Mobile() : Entity(), banked_ticks_(0), last_dir_(0)
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

// Moves in a given direction, or attacks something in the destination tile
bool Mobile::move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy)
{
    if (!dx && !dy)
    {
        core()->guru()->nonfatal("move_or_attack called with no direction!", Guru::GURU_WARN);
        return false;
    }
    const bool is_player = self->type() == EntityType::PLAYER;
    int xdx = x() + dx, ydy = y() + dy;
    auto area = core()->game()->area();
    if (area->can_walk(xdx, ydy))
    {
        auto the_tile = area->tile(xdx, ydy);
        const bool openable = (the_tile->tag(TileTag::Openable));
        const float movement_cost = (openable ? Timing::TIME_OPEN_DOOR : self->movement_speed());
        if (!is_player && banked_ticks() < movement_cost) return false;
        if (is_player) core()->game()->pass_time(movement_cost);
        else spend_banked_ticks(movement_cost);

        if (openable)
        {
            if (is_player) core()->message("You open the " + the_tile->name() + ".");
            else if (area->is_in_fov(xdx, ydy)) core()->message("{b}You see a " + the_tile->name() + " open.");
            auto tile = area->tile(xdx, ydy);
            tile->set_ascii('\'');
            tile->clear_tag(TileTag::Openable);
            tile->clear_tag(TileTag::BlocksLight);
            tile->set_tag(TileTag::Closeable);
            tile->set_tag(TileTag::Open);
            area->need_fov_recalc();
            return true;
        }

        set_pos(xdx, ydy);
        last_dir_ = ((dx + 2) << 4) + (dy + 2);
        area->need_fov_recalc();
        return true;
    }

    // We're not taking an action right now.
    if (!is_player) clear_banked_ticks();
    return false;
}

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
