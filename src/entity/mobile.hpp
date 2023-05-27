// entity/mobile.hpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_MOBILE_HPP_
#define ENTITY_MOBILE_HPP_

#include "entity/entity.hpp"


namespace invictus
{

// Used with certain functions when interacting with items.
enum class ItemLocation : uint8_t { GROUND, INVENTORY, EQUIPMENT };


class Mobile : public Entity
{
public:
                    Mobile();   // Constructor.
    virtual void    add_banked_ticks(float amount); // Adds or removes banked ticks to this Mobile.
    virtual float   banked_ticks() const;   // Retrieves the amount of ticks banked by this Mobile.
    virtual void    clear_banked_ticks();   // Erase all banked ticks on this Mobile.
    void            close_door(int dx, int dy); // Attempts to close a door.
    void            drop_item(uint32_t id); // Drops a carried item.
    bool            is_dead() const;        // Checks if this Mobile is dead.
    virtual bool    move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy);   // Moves in a given direction, or attacks something in the destination tile.
    float           movement_speed() const; // Returns the amount of ticks needed for this Mobile to move one tile.
    void            take_item(uint32_t id); // Picks up a specified item.
    void            tick(std::shared_ptr<Entity> self) override;    // Processes AI for this Mobile each turn.
    void            tick10(std::shared_ptr<Entity> self) override;  // Process slower state-change events that happen less often, such as buffs/debuffs ticking.
    void            timed_action(float time_taken); // This Mobile has made an action which takes time. Handles both Mobile and Player differences internally.
    EntityType      type() const { return EntityType::MOBILE; } // Self-identifier function.

    float   banked_ticks_;  // The amount of time this Mobile has 'banked'; it can 'spend' this time to move or attack.
    uint8_t last_dir_;      // The last direction this Mobile moved in.
};

}       // namespace invictus
#endif  // ENTITY_MOBILE_HPP_
