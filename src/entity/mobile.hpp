// entity/mobile.hpp -- The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef ENTITY_MOBILE_HPP_
#define ENTITY_MOBILE_HPP_

#include "entity/entity.hpp"


namespace invictus
{

enum class BuffType : uint16_t; // defdined in entity/buff.hpp
enum class ItemID : uint16_t;   // defined in codex/codex-item.hpp
class Buff;     // defined in entity/buff.hpp
class Player;   // defined in entity/player.hpp


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
    void            add_bloody_feet(float blood);   // Increase (or decrease) the amount of blood on this Mobile's feet.
    void            add_buff(BuffType type, int power, int duration, bool extend = true);   // Adds or extends the length of a Buff on this Mobile.
    virtual int     armour();   // Returns the total armour modifier from this Mobile and their equipped gear.
    float           attack_speed(); // Returns the number of ticks needed for this Mobile to make an attack.
    bool            blocks_tile(int x_tile, int y_tile) const override; // Checks if this Mobile blocks a specified tile.
    float           bloody_feet() const;    // Checks how bloodied this Mobile's feet are.
    void            close_door(int dx, int dy); // Attempts to close a door.
    void            die();          // Causes this Mobile to die!
    virtual int     dodge() = 0;    // Returns this Mobile's dodge score.
    void            drop_item(uint32_t id); // Drops a carried item.
    std::vector<std::shared_ptr<Item>>* equ();  // Retrieves a pointer to the equipment vector.
    void            equip_item(uint32_t id);    // Equips a specified Item.
    std::shared_ptr<Item>   equipment(EquipSlot slot);  // Retrieves equipment from a given slot.
    int             has_buff(BuffType type) const;      // Checks if this Mobile has a specified buff/debuff.
    uint16_t        hp(bool max = false) const; // Retrieves the current or maximum hit points of this Mobile.
    bool            is_awake() const;   // Check if this Mobile is awake and active.
    bool            is_dead() const;    // Checks if this Mobile is dead.
    virtual bool    move_or_attack(std::shared_ptr<Mobile> self, int dx, int dy);   // Moves in a given direction, or attacks something in the destination tile.
    float           movement_speed() const; // Returns the amount of ticks needed for this Mobile to move one tile.
    uint16_t        mp(bool max = false) const; // Retrieves the current or maximum mana points of this Mobile.
    void            set_equipment(EquipSlot slot, std::shared_ptr<Item> item);  // Manually equips an item.
    void            set_equipment(EquipSlot slot, ItemID id);   // As above, but generates a new Item from its ID.
    void            set_hp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's HP directly.
    void            set_hp_regen_speed(float regen_speed);  // Sets the HP regeneration speed for this Mobile.
    void            set_mp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's MP directly.
    void            set_sp(uint16_t current, uint16_t max = UINT16_MAX);    // Sets this Mobile's SP directly.
    void            sleep();    // Sends this Mobile to sleep.
    uint16_t        sp(bool max = false) const; // Retrieves the current or maximum stamina points of this Mobile.
    void            take_damage(int damage);    // Takes damage!
    void            take_item(uint32_t id); // Picks up a specified item.
    virtual void    tick(std::shared_ptr<Entity> self) override;    // Processes AI for this Mobile each turn.
    void            tick10(std::shared_ptr<Entity> self) override;  // Process slower state-change events that happen less often, such as buffs/debuffs ticking.
    void            tick_buffs(std::shared_ptr<Mobile> self);       // Ticks any buff/debuffs on this Mobile.
    virtual void    timed_action(float time_taken) = 0; // This Mobile has made an action which takes time.
    void            unequip_item(EquipSlot slot);   // Unequips a specified Item.
    virtual void    wake(); // Awakens this Mobile, if it's not already.
    void            wake_message(EnemyWakeMsg type, bool warning = false);  // Displays a message when an enemy awakes, or begins to awake.

private:
    static std::shared_ptr<Item>    blank_item_;    // A blank item, shared amongst all Mobiles to use in their empty equipment slots.

    bool        awake_;             // Is this Mobile active?
    float       bloody_feet_;       // How bloodied are this Mobile's feet?
    std::vector<std::shared_ptr<Buff>>  buffs_;     // Any buffs/debuffs on this Mobile.
    std::vector<std::shared_ptr<Item>>  equipment_; // Items equipped by this Mobile.
    uint16_t    hp_[2];             // The current and maximum hit points of this Mobile.
    float       move_speed_;        // The movement speed of this Mobile.
    uint16_t    mp_[2];             // The current and maximum mana points of this Mobile.
    float       regen_speed_[3];    // HP, SP and MP regeneration speed.
    float       regen_timer_[3];    // Builds up over time until the regeneration tick is triggered.
    uint16_t    sp_[2];             // The current and maximum stamina points of this Mobile.

friend class CodexMonster;
friend class SaveLoad;
};

}       // namespace invictus
#endif  // ENTITY_MOBILE_HPP_
