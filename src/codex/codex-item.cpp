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

        /****************************
         * BODY ARMOUR AND CLOTHING *
         ****************************/

        case ItemID::LEATHER_ARMOUR:    // The most basic tier of armour, adds protection but has no downsides.
            new_item->name_ = "leather armour";
            new_item->ascii_ = ASCII_ARMOUR;
            new_item->colour_ = Colour::YELLOW;
            new_item->item_type_ = ItemType::ARMOUR;
            new_item->item_subtype_ = ItemSub::BODY;
            new_item->set_tags({EntityTag::NoA, EntityTag::ProperNoun, EntityTag::ArmourLight});
            new_item->set_prop(EntityProp::ARMOUR, 11);
            break;

        case ItemID::RAGGED_ARMOUR: // Worn by zombies and such.
            new_item->name_ = "ragged armour";
            new_item->ascii_ = ASCII_ARMOUR;
            new_item->colour_ = Colour::YELLOW;
            new_item->item_type_ = ItemType::ARMOUR;
            new_item->item_subtype_ = ItemSub::BODY;
            new_item->set_tags({EntityTag::NoA, EntityTag::ProperNoun, EntityTag::ArmourLight});
            new_item->set_prop(EntityProp::ARMOUR, 8);
            break;

        /*****************
         * MELEE WEAPONS *
         *****************/

        case ItemID::SHORTSWORD:    // A simple one-handed blade.
            new_item->name_ = "shortsword";
            new_item->ascii_ = ASCII_EDGED_WEAPON;
            new_item->colour_ = Colour::WHITE;
            new_item->item_type_ = ItemType::WEAPON;
            new_item->item_subtype_ = ItemSub::SWORD;
            new_item->set_tag(EntityTag::WeaponFinesse);
            new_item->set_damage(1, 6);
            break;

        case ItemID::LONGSWORD: // A versatile blade that can be held in one or both hands.
            new_item->name_ = "longsword";
            new_item->ascii_ = ASCII_EDGED_WEAPON;
            new_item->colour_ = Colour::WHITE;
            new_item->item_type_ = ItemType::WEAPON;
            new_item->item_subtype_ = ItemSub::SWORD;
            new_item->set_damage(1, 8);
            new_item->set_tag(EntityTag::HandAndAHalf);
            break;

        case ItemID::GREATSWORD:    // A heavy sword that requires both hands to wield.
            new_item->name_ = "greatsword";
            new_item->ascii_ = ASCII_EDGED_WEAPON;
            new_item->colour_ = Colour::WHITE;
            new_item->item_type_ = ItemType::WEAPON;
            new_item->item_subtype_ = ItemSub::SWORD;
            new_item->set_damage(2, 6);
            new_item->set_tag(EntityTag::TwoHanded);
            break;


        /*******************
         * SPECIAL WEAPONS *
         *******************/

        case ItemID::UNARMED_ATTACK:    // Used for unarmed attacks, when the Mobile has no weapon equipped.
            new_item->name_ = "meaty fist";
            new_item->ascii_ = ASCII_BLUNT_WEAPON;
            new_item->colour_ = Colour::YELLOW;
            new_item->item_type_ = ItemType::WEAPON;
            new_item->item_subtype_ = ItemSub::UNARMED;
            new_item->set_damage(1, 2);
            break;
    }

    return new_item;
}

}   // namespace invictus
