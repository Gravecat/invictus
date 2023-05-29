// codex/codex-mobile.hpp -- Factory class, generating preset types of Mobiles that occupy the game world.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CODEX_CODEX_MOBILE_HPP_
#define CODEX_CODEX_MOBILE_HPP_

#include <cstdint>
#include <memory>


namespace invictus
{

class Mobile;   // defined in entity/mobile.hpp


enum class MobileID : uint16_t { NONE = 0, DRUJ_WALKER };


class CodexMobile
{
public:
    static std::shared_ptr<Mobile>  generate(MobileID id);  // Generates a new Mobile from a template.
};

}       // namespace invictus
#endif  // CODEX_CODEX_MOBILE_HPP_
