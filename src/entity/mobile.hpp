// entity/mobile.hpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_MOBILE_HPP_
#define ENTITY_MOBILE_HPP_

#include "entity/entity.hpp"


namespace invictus
{

class Mobile : public Entity
{
public:
                    Mobile();   // Constructor.
    void            add_banked_ticks(float amount); // Adds or removes banked ticks to this Mobile.
    float           banked_ticks() const;   // Retrieves the amount of ticks banked by this Mobile.
    void            clear_banked_ticks();   // Erase all banked ticks on this Mobile.
    void            close_door(int dx, int dy); // Attempts to close a door.
    bool            is_dead() const;        // Checks if this Mobile is dead.
    virtual bool    move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy);   // Moves in a given direction, or attacks something in the destination tile.
    float           movement_speed() const; // Returns the amount of ticks needed for this Mobile to move one tile.
    void            spend_banked_ticks(float amount);   // The exact opposite of add_banked_ticks().
    void            tick(std::shared_ptr<Entity> self) override;    // Processes AI for this Mobile each turn.
    void            tick10(std::shared_ptr<Entity> self) override;  // Process slower state-change events that happen less often, such as buffs/debuffs ticking.
    EntityType      type() const { return EntityType::MOBILE; } // Self-identifier function.

private:
    float   banked_ticks_;  // The amount of time this Mobile has 'banked'; it can 'spend' this time to move or attack.
    uint8_t last_dir_;      // The last direction this Mobile moved in.
};

};


#endif  // ENTITY_MOBILE_HPP_
