// codex/codex-monster.hpp -- Factory class, generating preset types of Monsters that occupy the game world.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CODEX_CODEX_MONSTER_HPP_
#define CODEX_CODEX_MONSTER_HPP_

#include <cstdint>
#include <memory>


namespace invictus
{

class Monster;  // defined in entity/monster.hpp


enum class MonsterID : uint16_t { NONE = 0, DRUJ_WALKER };


class CodexMonster
{
public:
    static std::shared_ptr<Monster> generate(MonsterID id); // Generates a new Monster from a template.
};

}       // namespace invictus
#endif  // CODEX_CODEX_MONSTER_HPP_
