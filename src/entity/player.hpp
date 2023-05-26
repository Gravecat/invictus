// entity/player.hpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use the same stats for combat etc.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_PLAYER_HPP_
#define ENTITY_PLAYER_HPP_

#include "entity/mobile.hpp"


namespace invictus
{

enum class ItemInteraction : uint8_t { DO_NOTHING, DROP };

class Player : public Mobile
{
public:
                Player();   // Constructor.
    void        close_a_door(); // Attempts to close a nearby door.
    uint16_t    fov_radius() const; // Calculates the player's field-of-view radius.
    void        get_direction(int *dx, int *dy) const;      // Gets a direction from the player.
    void        get_item();         // Picks something up off the ground.
    void        item_interaction(uint32_t id, ItemLocation loc);    // Interacts with an item.
    void        open_a_door();      // Attempts to open a nearby door.
    void        take_inventory(bool equipment = false);     // Interact with carried items.
    EntityType  type() const { return EntityType::PLAYER; } // Self-identifier function.
};

}       // namespace invictus
#endif  // ENTITY_PLAYER_HPP_
