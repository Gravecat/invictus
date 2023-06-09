// codex/codex-item.hpp -- Factory class, generating preset types of Items which can be used by the player or NPCs.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CODEX_CODEX_ITEM_HPP_
#define CODEX_CODEX_ITEM_HPP_

#include <cstdint>
#include <memory>


namespace invictus
{

class Item; // defined in entity/item.hpp

enum class ItemID : uint16_t { NOTHING = 0, LEATHER_ARMOUR, SHORTSWORD, LONGSWORD, GREATSWORD, UNARMED_ATTACK, RAGGED_ARMOUR, SHORTSWORD_TARNISHED,
    CROWN_OF_KINGS };


class CodexItem
{
public:
    static std::shared_ptr<Item>    generate(ItemID id);    // Generates a new Item from a template.
};

}       // namespace invictus
#endif  // CODEX_CODEX_ITEM_HPP_
