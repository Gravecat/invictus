// entity/player.hpp -- The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use some of the same code.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_PLAYER_HPP_
#define ENTITY_PLAYER_HPP_

#include "entity/mobile.hpp"


namespace invictus
{

enum class ItemInteraction : uint8_t { DO_NOTHING, DROP, TAKE, EQUIP, UNEQUIP };

class Player : public Mobile
{
public:
                Player();   // Constructor.
    int         armour() override;  // Returns the total armour modifier from this Player and their equipped gear.
    bool        blocks_tile(int x_tile, int y_tile) const override; // Checks if this Player blocks a specified tile.
    void        close_a_door(); // Attempts to close a nearby door.
    int         dodge() override;   // Returns this Player's dodge score.
    int8_t      finesse() const;    // Retrieves this Player's finesse attribute.
    uint16_t    fov_radius() const; // Calculates the player's field-of-view radius.
    void        get_direction(int *dx, int *dy) const;      // Gets a direction from the player.
    void        get_item();         // Picks something up off the ground.
    void        ground_items();     // Interact with items on the ground.
    int8_t      intellect() const;  // Retrieves thisPlayer's intellect attribute.
    void        item_interaction(uint32_t id, ItemLocation loc);    // Interacts with an item.
    int8_t      might() const;      // Retrieves this Player's might attribute.
    void        open_a_door();      // Attempts to open a nearby door.
    void        recalc_max_hp_mp_sp();  // Recalculates the maximum HP/SP/MP values, based on Strength, Finesse and Intellect.
    void        set_finesse(int8_t new_fin);    // Sets this Player's Finesse attribute.
    void        set_intellect(int8_t new_int);  // Sets this Player's Intellect attribute.
    void        set_might(int8_t new_mig);      // Sets this Player's Might attribute.
    void        take_inventory(bool equipment = false);     // Interact with carried items.
    void        timed_action(float time_taken) override;    // This Player has made an action which takes time.
    EntityType  type() const override { return EntityType::PLAYER; }    // Self-identifier function.

private:
    int8_t      finesse_;   // How agile and precise this Player is. This affects using finesse weapons, and dodge bonus to armour.
    int8_t      intellect_; // How smart and well-attuned with magic this Player is.
    int8_t      might_;     // How physicaly strong and fit this Player is. This affects using most melee weapons, and gives a bonus to armour.

friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_PLAYER_HPP_
