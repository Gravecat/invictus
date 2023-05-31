// entity/monster.hpp -- Derived from Mobile, Monster has extra stats and code that are unique to NPCs, and do not apply to the player.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_MONSTER_HPP_
#define ENTITY_MONSTER_HPP_

#include "entity/mobile.hpp"


namespace invictus
{

class Monster : public Mobile
{
public:
                Monster();  // Constructor.
    void        add_banked_ticks(float amount); // Adds or removes banked ticks to this Monster.
    float       banked_ticks() const;   // Retrieves the amount of ticks banked by this Monster.
    void        clear_banked_ticks();   // Erase all banked ticks on this Monster.
    int         dodge() override;       // Returns this Monster's dodge score.
    void        set_last_dir(uint8_t dir);  // Set the last direction moved.
    void        set_tracking_turns(int16_t turns);  // Sets this Monster's number of tracking turns.
    void        tick(std::shared_ptr<Entity> self) override;    // Processes AI for this Monster each turn.
    void        timed_action(float time_taken) override;    // This Monster has made an action which takes time.
    int8_t      to_damage_bonus() const;    // Retrieves this Monster's to-damage bonus.
    int8_t      to_hit_bonus() const;       // Retrieves this Monster's to-hit bonus.
    int16_t     tracking_turns() const;     // Checks how many tracking turns this Monster has left.
    EntityType  type() const override { return EntityType::MONSTER; }   // Self-identifier function.

private:
    float       banked_ticks_;      // The amount of time this Monster has 'banked'; it can 'spend' this time to move or attack.
    uint8_t     dodge_;             // The dodge value used by this Monster.
    uint8_t     last_dir_;          // The last direction this Monster moved in.
    int         player_last_seen_x_, player_last_seen_y_;   // The last x,Y coordinates we saw the player at.
    int8_t      to_damage_bonus_;   // The base to-damage bonus on this Monster.
    int8_t      to_hit_bonus_;      // The base to-hit bonus on this Monster.
    int16_t     tracking_turns_;    // How many turns are we tracking the player while they are out of LoS?

friend class CodexMonster;
friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_MONSTER_HPP_
