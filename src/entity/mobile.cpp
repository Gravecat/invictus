// entity/mobile.cpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/mobile.hpp"
#include "tune/ascii-symbols.hpp"
#include "tune/timing.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Constructor.
Mobile::Mobile() : Entity(), banked_ticks_(0), last_dir_(0)
{
    set_name("mobile");
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

// Attempts to close a door.
void Mobile::close_door(int dx, int dy)
{
    auto area = core()->game()->area();
    bool success = true;
    const bool is_player = (type() == EntityType::PLAYER);
    for (auto mobile : *area->entities())
    {
        if (mobile->is_at(dx, dy))
        {
            success = false;
            break;
        }
    }

    auto the_tile = area->tile(dx, dy);
    std::string door_name = the_tile->name();
    StrX::find_and_replace(door_name, " (open)", "");
    if (!success)
    {
        if (is_player) core()->message("{y}You can't close it, something seems to be blocking the " + door_name + ".");
        return;
    }

    if (is_player)
    {
        core()->game()->pass_time(Timing::TIME_CLOSE_DOOR);
        core()->message("You close the " + door_name + ".");
    }
    else
    {
        spend_banked_ticks(Timing::TIME_CLOSE_DOOR);
        if (area->is_in_fov(dx, dy)) core()->message("{U}You see a " + door_name + " close.");
    }
    
    the_tile->set_ascii(ASCII_DOOR_CLOSED);
    the_tile->set_tag(TileTag::Openable);
    the_tile->set_tag(TileTag::BlocksLight);
    the_tile->clear_tag(TileTag::Closeable);
    the_tile->clear_tag(TileTag::Open);
    area->need_fov_recalc();
}

// Checks if this Mobile is dead.
bool Mobile::is_dead() const { return false; }

// Moves in a given direction, or attacks something in the destination tile
bool Mobile::move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy)
{
    auto game = core()->game();

    if (!dx && !dy)
    {
        core()->guru()->nonfatal("move_or_attack called with no direction!", Guru::GURU_WARN);
        return false;
    }
    const bool is_player = self->type() == EntityType::PLAYER;
    int xdx = x() + dx, ydy = y() + dy;
    auto area = game->area();
    if (area->can_walk(xdx, ydy))
    {
        auto the_tile = area->tile(xdx, ydy);
        const bool openable = (the_tile->tag(TileTag::Openable));
        const float movement_cost = (openable ? Timing::TIME_OPEN_DOOR : self->movement_speed());
        if (!is_player && banked_ticks() < movement_cost) return false;
        if (is_player) game->pass_time(movement_cost);
        else spend_banked_ticks(movement_cost);

        if (openable)
        {
            if (is_player) core()->message("You open the " + the_tile->name() + ".");
            else if (area->is_in_fov(xdx, ydy)) core()->message("{U}You see a " + the_tile->name() + " open.");
            auto tile = area->tile(xdx, ydy);
            tile->set_ascii(ASCII_DOOR_OPEN);
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

        if (is_player)
        {
            std::vector<std::string> floor_items;
            for (auto entity : *area->entities())
            {
                if (entity == self || !entity->is_at(self)) continue;
                floor_items.push_back(entity->name());
            }
            if (floor_items.size()) core()->message("You see {c}" + StrX::comma_list(floor_items, true) + " {w}here.", 0);
            Tile* self_tile = area->tile(xdx, ydy);
            if (self_tile->tag(TileTag::StairsDown)) core()->message("You see a staircase leading downward.");
            else if (self_tile->tag(TileTag::StairsUp)) core()->message("You see a staircase leading upward.");
            else if (self_tile->tag(TileTag::Open))
            {
                std::string door_name = self_tile->name();
                StrX::find_and_replace(door_name, " (open)", "");
                core()->message("You pass through an open " + door_name + ".");
            }
        }

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

// Picks up a specified item.
void Mobile::take_item(uint32_t id)
{
    auto area = core()->game()->area();
    auto entities = area->entities();
    if (entities->size() <= id) core()->guru()->halt("Attempt to pick up invalid item ID.", id);
    std::shared_ptr<Entity> entity = entities->at(id);
    if (entity->type() != EntityType::ITEM) core()->guru()->halt("Attempt to pick up non-item entity.", id);

    inventory_add(entity);
    entities->erase(entities->begin() + id);
    if (type() == EntityType::PLAYER) core()->message("You pick up {c}" + entity->name() + "{w}.");
    else if (is_in_fov()) core()->message("{u}" + name() + " {u}picks up " + entity->name() + "{u}.");
}

// Processes AI for this Mobile each turn.
void Mobile::tick(std::shared_ptr<Entity> self)
{
    Entity::tick(self);
    if (is_dead()) return;

    if (type() == EntityType::PLAYER) return;

    add_banked_ticks(Timing::TICK_SPEED);
    auto self_mob = std::dynamic_pointer_cast<Mobile>(self);

    // There's nothing more to do.
    clear_banked_ticks();
}

// Process slower state-change events that happen less often, such as buffs/debuffs ticking.
void Mobile::tick10(std::shared_ptr<Entity> self)
{
    Entity::tick10(self);
    if (is_dead()) return;
}

}   // namespace invictus
