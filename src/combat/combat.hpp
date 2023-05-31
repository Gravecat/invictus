
// combat/combat.hpp -- The core of the combat engine, allowing the player and NPCs to fight each other.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef COMBAT_COMBAT_HPP_
#define COMBAT_COMBAT_HPP_

#include <cstdint>
#include <memory>
#include <string>


namespace invictus
{

class Mobile;   // defined in entity/mobile.hpp
enum class EquipSlot : uint8_t; // defined in entity/mobile.hpp


enum class WieldType : uint8_t { NONE, UNARMED, ONE_HAND_PLUS_EXTRA, TWO_HAND, DUAL_WIELD, HAND_AND_A_HALF_2H, SINGLE_WIELD, ONE_HAND_PLUS_SHIELD, SHIELD_ONLY,
    RANGED_ATTACK };


class Combat
{
public:
    static bool bump_attack(std::shared_ptr<Mobile> attacker, std::shared_ptr<Mobile> defender);    // The humble bump attack, a staple of roguelikes everywhere
    static void determine_wield_type(std::shared_ptr<Mobile> mob, bool is_attacker, WieldType *wield_type, bool *can_main_attack = nullptr,
        bool *can_off_attack = nullptr);    // Determines the type of weapons wielded by a Mobile.
    static void perform_attack(std::shared_ptr<Mobile> attacker, std::shared_ptr<Mobile> defender, EquipSlot weapon, WieldType wield_type_attacker,
        WieldType wield_type_defender);	// Performs an attack with a single weapon.

private:
    static std::string  damage_number_str(int damage, int blocked, bool crit, bool bleed, bool poison); // Generates a standard-format damage number string.
    static std::string  damage_str(int damage, std::shared_ptr<Mobile> defender, bool heat);    // Returns an appropriate damage string.
    // Returns a threshold string, if a damage threshold has been passed.
    static std::string  threshold_str(std::shared_ptr<Mobile> defender, int damage, const std::string &good_colour, const std::string &bad_colour);
};

}       // namespace invictus
#endif  // COMBAT_COMBAT_HPP_
