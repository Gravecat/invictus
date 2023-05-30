// codex/codex-mobile.cpp -- Factory class, generating preset types of Mobiles that occupy the game world.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "codex/codex-mobile.hpp"
#include "entity/mobile.hpp"
#include "terminal/terminal-shared-defs.hpp"
#include "tune/ascii-symbols.hpp"


namespace invictus
{

// Generates a new Mobile from a template.
std::shared_ptr<Mobile> CodexMobile::generate(MobileID id)
{
    std::shared_ptr<Mobile> mob = std::make_shared<Mobile>();
    switch(id)
    {
        case MobileID::NONE: break;

        case MobileID::DRUJ_WALKER:
            mob->name_ = "druj walker";
            mob->ascii_ = ASCII_ZOMBIE;
            mob->colour_ = Colour::CYAN;
            mob->set_tags({EntityTag::Unliving, EntityTag::ImmunityPoison});
            break;
    }

    return mob;
}

}   // namespace invictus
