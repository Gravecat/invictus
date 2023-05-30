// tune/combat.hpp -- Predefined variables which can be tweaked to rebalance aspects of combat.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_COMBAT_HPP_
#define TUNE_COMBAT_HPP_

namespace invictus
{

constexpr float CRITICAL_HIT_DAMAGE_MULTI =             2.0f;   // The damage multiplier when getting a critical hit.
constexpr int   DUAL_WIELD_TO_HIT_MOD =                 -2; // The modifier to to-hit chance when dual-wielding one-handed weapons.
constexpr int   HAND_AND_A_HALF_SINGLE_HAND_HIT_MOD =   -1; // The modifier to to-hit chance when single-wielding a hand-and-a-half weapon.
constexpr int   SINGLE_WIELD_HIT_MOD =                  3;  // The modifier to to-hit chance when single-wielding a one-handed weapon.
constexpr int   SWORD_AND_BOARD_HIT_MULTI =             0;  // The modifier to to-hit chance when wielding a one-handed weapon plus a shield.

}       // namespace invictus
#endif  // TUNE_COMBAT_HPP_
