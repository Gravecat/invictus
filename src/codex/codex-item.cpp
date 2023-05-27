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

        case ItemID::TEST_ITEM: // This is just used for testing, and will likely be removed later.
            new_item->name_ = "interesting rock";
            new_item->ascii_ = ASCII_STONE;
            new_item->colour_ = Colour::GREEN;
            new_item->item_type_ = ItemType::JUNK;
            break;

        case ItemID::LEATHER_ARMOUR:    // The most basic tier of armour, adds protection but has no downsides.
            new_item->name_ = "leather armour";
            new_item->ascii_ = ASCII_ARMOUR;
            new_item->colour_ = Colour::YELLOW;
            new_item->item_type_ = ItemType::ARMOUR;
            new_item->item_subtype_ = ItemSub::BODY;
            new_item->set_tags({EntityTag::NoA, EntityTag::ProperNoun});
            new_item->set_props({{EntityProp::ARMOUR, 11}, {EntityProp::MAX_DEX, 4}});
            break;
    }

    return new_item;
}

}   // namespace invictus
