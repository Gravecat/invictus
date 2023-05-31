// tune/combat.hpp -- Predefined variables which can be tweaked to rebalance aspects of combat.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_COMBAT_HPP_
#define TUNE_COMBAT_HPP_

namespace invictus
{

constexpr int   ARMOUR_PER_MIGHT =              2;      // The armour score bonus, per point of Might.
constexpr int   BASE_HIT_POINTS =               50;     // The base value of hit points, before Might bonuses.
constexpr int   BASE_MANA_POINTS =              0;      // The base value of mana points, before Intellect bonuses.
constexpr int   BASE_STAMINA_POINTS =           25;     // The base value of stamina points, before Finesse and Might bonuses.
constexpr int   CRITICAL_HIT_DAMAGE_MULTI =     2;      // The damage multiplier when getting a critical hit.
constexpr int   DEATH_SCREAM_DISTANCE =         10;     // The distance that the player can hear mobile deaths when outside of line-of-sight.
constexpr bool  DEBUG_VIEW_COMBAT_NUMBERS =     true;  // Views the raw rolls in combat.
constexpr int   DODGE_PER_FINESSE =             1;      // The dodge score and light armour score bonus, per point of Finesse.
constexpr int   DUAL_WIELD_TO_HIT_MOD =         -4;     // The penalty for to-hit chance when dual-wielding a non-finesse/non-light melee weapon.
constexpr int   DUAL_WIELD_TO_HIT_MOD_FINESSE = -2;     // As above, but for a finesse melee weapon.
constexpr int   DUAL_WIELD_TO_HIT_MOD_LIGHT =   0;      // As above, but for a light melee weapon.
constexpr int   FINESSE_DAMAGE_BONUS =          2;      // The bonus per point of Finesse to damage, when using finesse or ranged weapons.
constexpr int   FINESSE_HIT_BONUS =             2;      // The bonus per point of Finesse to to-hit chance, when using finesse or ranged weapons.
constexpr int   HIT_POINTS_PER_MIGHT =          5;      // Bonus hit points per point of Might.
constexpr int   MANA_PER_INTELLECT =            15;     // The bonus mana points per point of Intellect.
constexpr int   MIGHT_DAMAGE_BONUS =            2;      // The bonus per point of Might to damage, when using melee weapons.
constexpr int   MIGHT_HIT_BONUS =               2;      // The bonus per point of Might to to-hit chance, when using melee weapons.
constexpr int   SINGLE_WIELD_HIT_MOD =          2;      // The bonus to to-hit chance, when single-wielding a melee weapon.
constexpr int   SINGLE_WIELD_HIT_MOD_FINESSE =  4;      // As above, but for finesse weapons.
constexpr int   STAMINA_PER_FINESSE =           10;     // The bonus stamina points per point of Finesse.
constexpr int   STAMINA_PER_MIGHT =             5;      // The bonus stamina points per point of Might.

}       // namespace invictus
#endif  // TUNE_COMBAT_HPP_
