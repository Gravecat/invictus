// tune/combat.hpp -- Predefined variables which can be tweaked to rebalance aspects of combat.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_COMBAT_HPP_
#define TUNE_COMBAT_HPP_

namespace invictus
{

constexpr int   ARMOUR_PER_MIGHT =              2;      // The armour score bonus, per point of Might.
constexpr float CRITICAL_HIT_DAMAGE_MULTI =     2.0f;   // The damage multiplier when getting a critical hit.
constexpr bool  DEBUG_VIEW_COMBAT_NUMBERS =     true;   // Views the raw rolls in combat.
constexpr int   DODGE_PER_FINESSE =             1;      // The dodge score and light armour score bonus, per point of Finesse.
constexpr int   DUAL_WIELD_TO_HIT_MOD =         -4;     // The penalty for to-hit chance when dual-wielding a non-finesse/non-light melee weapon.
constexpr int   DUAL_WIELD_TO_HIT_MOD_FINESSE = -2;     // As above, but for a finesse melee weapon.
constexpr int   DUAL_WIELD_TO_HIT_MOD_LIGHT =   0;      // As above, but for a light melee weapon.
constexpr int   FINESSE_DAMAGE_BONUS =          2;      // The bonus per point of Finesse to damage, when using finesse or ranged weapons.
constexpr int   FINESSE_HIT_BONUS =             2;      // The bonus per point of Finesse to to-hit chance, when using finesse or ranged weapons.
constexpr int   MIGHT_DAMAGE_BONUS =            2;      // The bonus per point of Might to damage, when using melee weapons.
constexpr int   MIGHT_HIT_BONUS =               2;      // The bonus per point of Might to to-hit chance, when using melee weapons.
constexpr int   SINGLE_WIELD_HIT_MOD =          2;      // The bonus to to-hit chance, when single-wielding a melee weapon.
constexpr int   SINGLE_WIELD_HIT_MOD_FINESSE =  4;      // As above, but for finesse weapons.

}       // namespace invictus
#endif  // TUNE_COMBAT_HPP_
