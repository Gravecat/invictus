// codex/codex-item.cpp -- Factory class, generating preset types of Items which can be used by the player or NPCs.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "codex/codex-item.hpp"
#include "entity/item.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"


namespace invictus
{

// Generates a new Item from a template.
std::shared_ptr<Item> CodexItem::generate(ItemID id)
{
    std::shared_ptr<Item> new_item = std::make_shared<Item>();
    switch(id)
    {
        case ItemID::NOTHING: break;

        case ItemID::TEST_ITEM:
            new_item->name_ = "interesting rock";
            new_item->ascii_ = ASCII_STONE;
            new_item->colour_ = Colour::GREEN;
            break;
    }

    return new_item;
}

}   // namespace invictus
