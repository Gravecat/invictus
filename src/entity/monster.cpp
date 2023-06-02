// entity/monster.cpp -- Derived from Mobile, Monster has extra stats and code that are unique to NPCs, and do not apply to the player.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "area/area.hpp"
#include "area/pathfind.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "entity/monster.hpp"
#include "entity/player.hpp"
#include "tune/ai.hpp"
#include "tune/timing.hpp"
#include "util/random.hpp"


namespace invictus
{

// Constructor.
Monster::Monster() : Mobile(), banked_ticks_(0), dodge_(10), last_dir_(0), player_last_seen_x_(-1), player_last_seen_y_(-1), to_damage_bonus_(0),
    to_hit_bonus_(0), tracking_turns_(0)
{ set_name("monster"); }

// Adds or removes banked ticks to this Monster.
void Monster::add_banked_ticks(float amount)
{
    if (amount < 0) core()->guru()->halt("Attempt to add negative banked ticks to " + name(), static_cast<int>(amount));
    else banked_ticks_ += amount;
}

// Retrieves the amount of ticks banked by this Monster.
float Monster::banked_ticks() const { return banked_ticks_; }

// Erase all banked ticks on this Monster.
void Monster::clear_banked_ticks() { banked_ticks_ = 0; }

// Returns this Monster's dodge score.
int Monster::dodge() {  return dodge_; }

// Set the last direction moved.
void Monster::set_last_dir(uint8_t dir) { last_dir_ = dir; }

// Sets this Monster's number of tracking turns.
void Monster::set_tracking_turns(int16_t turns)
{
    if (turns < 0) tracking_turns_ += turns;
    else tracking_turns_ = turns;
}

// Processes AI for this Monster each turn.
void Monster::tick(std::shared_ptr<Entity> self)
{
    Mobile::tick(self);
    if (is_dead()) return;

    if (!is_awake())
    {
        clear_banked_ticks();
        return;
    }
    add_banked_ticks(TICK_SPEED);
    auto area = core()->game()->area();

    if (tag(EntityTag::Passive))    // Passive Monsters have no wish to attack the player.
    {
        clear_banked_ticks();
        return;
    }

    // If there aren't enough banked ticks to act, do nothing.
    if (banked_ticks_ < movement_speed() && banked_ticks_ < attack_speed()) return;

    if (!tag(EntityTag::Blind) && is_in_fov()) // Non-blind Monsters will hunt the player by sight.
    {
        auto player = core()->game()->player();
        set_tracking_turns(AI_TRACKING_TURNS);
        player_last_seen_x_ = player->x();
        player_last_seen_y_ = player->y();
        auto pathfind = std::make_shared<Pathfind>(PathfindMode::PATHFIND_MONSTER, x(), y(), player_last_seen_x_, player_last_seen_y_);
        auto result = pathfind->pathfind();
        if (!result.size())
        {
            clear_banked_ticks();   // Can't find any route, so just do nothing.
            return;
        }

        // Find the next step in the path.
        int next_x = result.at(0).first, next_y = result.at(0).second;

        // Check to see if anything is blocking the way.
        for (auto entity : *core()->game()->area()->entities())
        {
            if (entity == self || entity->type() == EntityType::PLAYER) continue;
            if (entity->blocks_tile(next_x, next_y))
            {
                clear_banked_ticks();
                return; // We're waiting patiently.
            }
        }

        // Looks like we're good to go!
        move_or_attack(std::dynamic_pointer_cast<Mobile>(self), next_x - x(), next_y - y());
        return;
    }

    // The player is out of sight, but we're still going to try to find them.
    if (tracking_turns())
    {
        if (banked_ticks() < movement_speed()) return;  // Do nothing if we don't have enough banked ticks.
        set_tracking_turns(-1);

        int dx = 0, dy = 0;

        // If possible, move to the last place we saw the player.
        if (player_last_seen_x_ >= 0 && player_last_seen_y_ >= 0)
        {
            // If we've reached the last-seen location, erase it.
            if (player_last_seen_x_ == x() && player_last_seen_y_ == y())
                player_last_seen_x_ = player_last_seen_y_ = -1;
            else
            {
                // Pathfind to the player's last seen location.
                auto pathfind = std::make_shared<Pathfind>(PathfindMode::PATHFIND_MONSTER, x(), y(), player_last_seen_x_, player_last_seen_y_);
                auto result = pathfind->pathfind();
                if (result.size())
                {
                    // Find the next step in the path.
                    int next_x = result.at(0).first, next_y = result.at(0).second;
                    dx = next_x - x();
                    dy = next_y - y();
                }
                else
                {
                    // If the path is no longer viable, forget about it.
                    player_last_seen_x_ = player_last_seen_y_ = 0;
                }
            }
        }

        // We've been unable to pathfind to the player's last location, or have reached that location and the player is not in sight.
        if (dx == 0 && dy == 0)
        {
            // First, determine which directions are viable to search. This includes any direction BUT the one we just came from.
            std::vector<int> viable_directions;
            for (int vx = -1; vx <= 1; vx++)
            {
                for (int vy = -1; vy <= 1; vy++)
                {
                    if (vx == 0 && vy == 0) continue;
                    int vc = ((vx + 2) << 4) + (vy + 2);
                    if (vc == last_dir_) continue;
                    if (area->can_walk(x() + vx, y() + vy)) viable_directions.push_back(vc);
                }
            }
            if (!viable_directions.size())
            {
                // See if the way we came from is acceptable.
                int old_dy = (last_dir_ & 0xF) - 2;
                int old_dx = ((last_dir_ & 0xF0) >> 4) - 2;
                if (area->can_walk(x() + old_dx, y() + old_dy)) viable_directions.push_back(last_dir_);
                else
                {
                    clear_banked_ticks();
                    return;
                }
            }

            // There's now at least one viable option. Let's choose one randomly from the available list.
            int choice = Random::rng(0, viable_directions.size() - 1);
            dy = (viable_directions.at(choice) & 0xF) - 2;
            dx = ((viable_directions.at(choice) & 0xF0) >> 4) - 2;
        }

        move_or_attack(std::dynamic_pointer_cast<Mobile>(self), dx, dy);
        return;
    }

    // There's nothing more to do.
    clear_banked_ticks();
}

// This Monster has made an action which takes time.
void Monster::timed_action(float time_taken) { banked_ticks_ -= time_taken; }

// Retrieves this Monster's to-damage bonus.
int8_t Monster::to_damage_bonus() const { return to_damage_bonus_; }

// Retrieves this Monster's to-hit bonus.
int8_t Monster::to_hit_bonus() const { return to_hit_bonus_; }

// Checks how many tracking turns this Monster has left.
int16_t Monster::tracking_turns() const { return tracking_turns_; }

}   // namespace invictus
