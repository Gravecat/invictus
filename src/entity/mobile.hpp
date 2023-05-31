// entity/mobile.hpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_MOBILE_HPP_
#define ENTITY_MOBILE_HPP_

#include "entity/entity.hpp"


namespace invictus
{

// Messages that are shown when certain enemy types 'wake up'.
enum class EnemyWakeMsg : uint16_t { NONE, DRUJ_TOMB };

// Slots that can be used to equip items. _END is important, so we know how large to make the equipment vector.
enum class EquipSlot : uint8_t { HAND_MAIN, HAND_OFF, BODY, HEAD, HANDS, FEET, _END };

// Used with certain functions when interacting with items.
enum class ItemLocation : uint8_t { GROUND, INVENTORY, EQUIPMENT };


class Mobile : public Entity
{
public:
                    Mobile();   // Constructor.
    virtual void    add_banked_ticks(float amount); // Adds or removes banked ticks to this Mobile.
    void            add_bloody_feet(float blood);   // Increase (or decrease) the amount of blood on this Mobile's feet.
    virtual int     armour();   // Returns the total armour modifier from this Mobile and their equipped gear.
    float           attack_speed(); // Returns the number of ticks needed for this Mobile to make an attack.
    virtual float   banked_ticks() const;   // Retrieves the amount of ticks banked by this Mobile.
    bool            blocks_tile(int x_tile, int y_tile) const override; // Checks if this Mobile blocks a specified tile.
    float           bloody_feet() const;    // Checks how bloodied this Mobile's feet are.
    virtual void    clear_banked_ticks();   // Erase all banked ticks on this Mobile.
    void            close_door(int dx, int dy); // Attempts to close a door.
    void            die();      // Causes this Mobile to die!
    virtual int     dodge();    // Returns this Mobile's dodge score.
    void            drop_item(uint32_t id); // Drops a carried item.
    std::vector<std::shared_ptr<Item>>* equ();  // Retrieves a pointer to the equipment vector.
    void            equip_item(uint32_t id);    // Equips a specified Item.
    std::shared_ptr<Item>   equipment(EquipSlot slot);  // Retrieves equipment from a given slot.
    uint16_t        hp(bool max = false) const; // Retrieves the current or maximum hit points of this Mobile.
    bool            is_awake() const;   // Check if this Mobile is awake and active.
    bool            is_dead() const;    // Checks if this Mobile is dead.
    virtual bool    move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy);   // Moves in a given direction, or attacks something in the destination tile.
    float           movement_speed() const; // Returns the amount of ticks needed for this Mobile to move one tile.
    uint16_t        mp(bool max = false) const; // Retrieves the current or maximum mana points of this Mobile.
    void            set_hp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's HP directly.
    void            set_mp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's MP directly.
    void            set_sp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's SP directly.
    void            set_tracking_turns(int16_t turns);  // Sets this Mobile's number of tracking turns.
    uint16_t        sp(bool max = false) const; // Retrieves the current or maximum stamina points of this Mobile.
    void            take_damage(int damage);    // Takes damage!
    void            take_item(uint32_t id); // Picks up a specified item.
    void            tick(std::shared_ptr<Entity> self) override;    // Processes AI for this Mobile each turn.
    void            tick10(std::shared_ptr<Entity> self) override;  // Process slower state-change events that happen less often, such as buffs/debuffs ticking.
    void            timed_action(float time_taken); // This Mobile has made an action which takes time. Handles both Mobile and Player differences internally.
    int8_t          to_damage_bonus() const;    // Retrieves this Mobile's to-damage bonus.
    int8_t          to_hit_bonus() const;       // Retrieves this Mobile's to-hit bonus.
    int16_t         tracking_turns() const; // Checks how many tracking turns this Mobile has left.
    EntityType      type() const { return EntityType::MOBILE; } // Self-identifier function.
    void            unequip_item(EquipSlot slot);   // Unequips a specified Item.
    void            wake(); // Awakens this Mobile, if it's not already.
    void            wake_message(EnemyWakeMsg type, bool warning = false);  // Displays a message when an enemy awakes, or begins to awake.

private:
    static std::shared_ptr<Item>    blank_item_;    // A blank item, shared amongst all Mobiles to use in their empty equipment slots.

    // The following are shared by both Mobile and its derived class, Player.
    bool        awake_;             // Is this Mobile active?
    float       bloody_feet_;       // How bloodied are this Mobile's feet?
    std::vector<std::shared_ptr<Item>>  equipment_; // Items equipped by this Mobile.
    uint16_t    hp_[2];             // The current and maximum hit points of this Mobile.
    uint8_t     last_dir_;          // The last direction this Mobile moved in.
    uint16_t    mp_[2];             // The current and maximum mana points of this Mobile.
    uint16_t    sp_[2];             // The current and maximum stamina points of this Mobile.

    // The following are ONLY used by Mobile and are not used for anything on Player.
    float       banked_ticks_;      // The amount of time this Mobile has 'banked'; it can 'spend' this time to move or attack.
    uint8_t     dodge_;             // The dodge value used by this Mobile.
    int         player_last_seen_x_, player_last_seen_y_;   // The last x,Y coordinates we saw the player at.
    int8_t      to_damage_bonus_;   // The base to-damage bonus on this Mobile (NOT used in calculations on Player!)
    int8_t      to_hit_bonus_;      // The base to-hit bonus on this Mobile (NOT used in calculations on Player!)
    int16_t     tracking_turns_;    // How many turns are we tracking the player while they are out of LoS?

friend class CodexMobile;
friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_MOBILE_HPP_
