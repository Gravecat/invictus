
// combat/combat.cpp -- The core of the combat engine, allowing the player and NPCs to fight each other.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>
#include <stdexcept>

#include "codex/codex-item.hpp"
#include "combat/combat.hpp"
#include "core/core.hpp"
#include "entity/item.hpp"
#include "entity/mobile.hpp"
#include "tune/combat.hpp"
#include "tune/resting.hpp"
#include "util/random.hpp"
#include "util/strx.hpp"


namespace invictus
{

// The humble bump attack, a staple of roguelikes everywhere.
bool Combat::bump_attack(std::shared_ptr<Mobile> attacker, std::shared_ptr<Mobile> defender)
{
	if (attacker->is_dead() || defender->is_dead()) return false;

	// We'll need to check the equipment for both attacker and defender, so we can determine how both are wielding their weapons.
	std::shared_ptr<Item> main_hand[2], off_hand[2];
	bool main_can_attack[2], off_can_attack[2];
	WieldType wield_type[2];

	for (int i = 0; i < 2; i++)
	{
		std::shared_ptr<Mobile> mob = (i == 0 ? attacker : defender);
		determine_wield_type(mob, (i == 0), &wield_type[i], &main_can_attack[i], &off_can_attack[i]);
		if (wield_type[i] == WieldType::NONE) return false;	// Just give up here if the attacker can't attack.
	}

	if (main_can_attack[0]) perform_attack(attacker, defender, EquipSlot::HAND_MAIN, wield_type[0], wield_type[1]);
	if (off_can_attack[0] && !attacker->is_dead() && !defender->is_dead()) perform_attack(attacker, defender, EquipSlot::HAND_OFF, wield_type[0],
        wield_type[1]);
	return true;
}

// Generates a standard-format damage number string.
std::string Combat::damage_number_str(int damage, int blocked, bool crit, bool bleed, bool poison)
{
    std::string dmg_str;
    if (crit) dmg_str = "{w}[{m}*"; else dmg_str = "{w}[";
    dmg_str += StrX::intostr_pretty(damage);
    if (bleed && !crit) dmg_str += "B";
    if (poison && !crit) dmg_str += "P";
    if (crit) dmg_str += "{m}*{w}]";
    else dmg_str += "]";
    if (blocked > 0) dmg_str += " {w}<{b}-" + StrX::intostr_pretty(blocked) + "{w}>";
    return dmg_str;
}

// Returns an appropriate damage string.
std::string Combat::damage_str(int damage, std::shared_ptr<Mobile> defender, bool heat)
{
    const float percentage = std::round((static_cast<float>(damage) / static_cast<float>(defender->hp(true))) * 100);
    if (percentage >= 200000) return StrX::rainbow_text("SUPERNOVAS", "rym");
    else if (percentage >= 150000) return StrX::rainbow_text("METEORITES", "urm");
    else if (percentage >= 125000) return StrX::rainbow_text("GLACIATES", "cuw");
    else if (percentage >= 100000) return StrX::rainbow_text("NUKES", "rmy");
    else if (percentage >= 80000) return StrX::rainbow_text("RUPTURES", "rmr");
    else if (percentage >= 65000) return StrX::rainbow_text("SLAUGHTERS", "mry");
    else if (percentage >= 50000) return StrX::rainbow_text("SHATTERS", "gcu");
    else if (percentage >= 40000) return StrX::rainbow_text("EXTERMINATES", "gyc");
    else if (percentage >= 30000) return StrX::rainbow_text("IMPLODES", "umr");
    else if (percentage >= 20000) return StrX::rainbow_text("ANNIHILATES", "rgu");
    else if (percentage >= 15000) return StrX::rainbow_text("CREMATES", "ywr");
    else if (percentage >= 12500) return StrX::rainbow_text("WASTES", "muc");
    else if (percentage >= 10000) return StrX::rainbow_text("TEARS INTO", "wrm");
    else if (percentage >= 9000) return StrX::rainbow_text("SUNDERS", "cmr");
    else if (percentage >= 8000) return StrX::rainbow_text("EVAPORATES", "ycu");
    else if (percentage >= 7000) return StrX::rainbow_text("LIQUIDATES", "cwu");
    else if (percentage >= 6000) return StrX::rainbow_text("FISSURES", "rum");
    else if (percentage >= 5000) return StrX::rainbow_text("RAVAGES", "myr");
    else if (percentage >= 4000) return StrX::rainbow_text("ASPHYXIATES", "mcu");
    else if (percentage >= 3000) return StrX::rainbow_text("ATOMIZES", "cyg");
    else if (percentage >= 2500) return StrX::rainbow_text("VAPORIZES", "yuc");
    else if (percentage >= 2000) return StrX::rainbow_text("PULVERIZES", "mrm");
    else if (percentage >= 1800) return StrX::rainbow_text("DESTROYS", "umw");
    else if (percentage >= 1600) return StrX::rainbow_text("SHREDS", "mru");
    else if (percentage >= 1400) return StrX::rainbow_text("DEMOLISHES", "uym");
    else if (percentage >= 1200) return StrX::rainbow_text("BLASTS", "ryw");
    else if (percentage >= 1000) return StrX::rainbow_text("RENDS", "rmu");
    else if (percentage >= 900) return StrX::rainbow_text("DISMEMBERS", "mrm");
    else if (percentage >= 800) return StrX::rainbow_text("MASSACRES", "rmr");
    else if (percentage >= 700) return StrX::rainbow_text("DISEMBOWELS", "mru");
    else if (percentage >= 600) return StrX::rainbow_text("MUTILATES", "urm");
    else if (percentage >= 500) return StrX::rainbow_text("INCINERATES", "ryw");
    else if (percentage >= 400) return StrX::rainbow_text("EXTIRPATES", "gcu");
    else if (percentage >= 300) return StrX::rainbow_text("OBLITERATES", "muc");
    else if (percentage >= 200) return StrX::rainbow_text("ERADICATES", "umr");
    else if (percentage >= 150) return StrX::rainbow_text("DEVASTATES", "ygc");
    else if (percentage >= 100) return StrX::rainbow_text("DECIMATES", "wyr");
    else if (percentage >= 90) return StrX::rainbow_text("LACERATES", "rmr");
    else if (percentage >= 80) return "{r}mars";
    else if (percentage >= 70) return "{r}mangles";
    else if (percentage >= 60) return "{r}maims";
    else if (percentage >= 50) return "{r}mauls";
    else if (percentage >= 40) return "{r}wounds";
    else if (percentage >= 30) return "{y}injures";
    else if (percentage >= 25) return "{y}damages";
    else if (percentage >= 20) return "{y}scars";
    else if (heat)
    {
        if (percentage >= 15) return "{w}scorches";
        else if (percentage >= 10) return "{w}chars";
        else if (percentage >= 5) return "{w}sears";
        else if (percentage >= 1) return "{w}scalds";
        else return "{w}singes";
    }
    else
    {
        if (percentage >= 15) return "{w}nicks";
        else if (percentage >= 10) return "{w}grazes";
        else if (percentage >= 5) return "{w}scratches";
        else if (percentage >= 1) return "{w}bruises";
        else return "{w}tickles";
    }
}

// Determines the type of weapons wielded by a Mobile.
void Combat::determine_wield_type(std::shared_ptr<Mobile> mob, bool is_attacker, WieldType *wield_type, bool *can_main_attack, bool *can_off_attack)
{
    std::shared_ptr<Item> main_hand = mob->equipment(EquipSlot::HAND_MAIN), off_hand = mob->equipment(EquipSlot::HAND_OFF);
    bool main_can_attack = (main_hand->item_type() == ItemType::WEAPON && main_hand->item_type() != ItemType::NONE && !main_hand->tag(EntityTag::WeaponRanged));
    bool off_can_attack = (off_hand->item_type() == ItemType::WEAPON && off_hand->item_type() != ItemType::NONE && !off_hand->tag(EntityTag::WeaponRanged));

    // Check for unarmed combat.
    if (main_hand->item_type() == ItemType::NONE && !off_can_attack)
    {
        main_can_attack = true;
        main_hand = CodexItem::generate(ItemID::UNARMED_ATTACK);
    }

    // Just give up here if the attacker can't attack.
    if (is_attacker && !main_can_attack && !off_can_attack)
    {
        *wield_type = WieldType::NONE;
        if (can_main_attack) *can_main_attack = false;
        if (can_off_attack) *can_off_attack = false;
        return;
    }

    *wield_type = WieldType::ONE_HAND_PLUS_EXTRA;   // default to one-handed + something else in the other hand
    if (main_hand->item_subtype() == ItemSub::UNARMED) *wield_type = WieldType::UNARMED;    // unarmed attack
    else if (main_can_attack && main_hand->tag(EntityTag::TwoHanded)) *wield_type = WieldType::TWO_HAND;    // two-handeders
    else if (main_can_attack && off_can_attack) *wield_type = WieldType::DUAL_WIELD;    // dual-wielding
    else if (main_can_attack && off_hand->item_type() == ItemType::NONE)
    {
        if (main_hand->tag(EntityTag::HandAndAHalf)) *wield_type = WieldType::HAND_AND_A_HALF_2H;   // hand-and-a-half in two-handed mode
        else *wield_type = WieldType::SINGLE_WIELD; // single-wielding a one-handed weapon
    }
    else if (main_can_attack && off_hand->item_type() == ItemType::SHIELD) *wield_type = WieldType::ONE_HAND_PLUS_SHIELD;   // one-hander + shield
    else if (!main_can_attack && off_can_attack) *wield_type = WieldType::SINGLE_WIELD;
    else if (!is_attacker)  // Special rules only for defenders.
    {
        // If the defender has a shield equipped but no weapon, they can still shield-block.
        if (off_hand->item_type() == ItemType::SHIELD) *wield_type = WieldType::SHIELD_ONLY;
        else if (!main_can_attack && !off_can_attack) *wield_type = WieldType::UNARMED; // Just count as unarmed if we're not wielding any viable melee weapons.
        else throw std::runtime_error("Cannot determine wield style for " + mob->name());
    }
    else throw std::runtime_error("Cannot determine wield style for " + mob->name());

    if (can_main_attack) *can_main_attack = main_can_attack;
    if (can_off_attack) *can_off_attack = off_can_attack;
}

// Performs an attack with a single weapon.
void Combat::perform_attack(std::shared_ptr<Mobile> attacker, std::shared_ptr<Mobile> defender, EquipSlot weapon, WieldType wield_type_attacker,
        WieldType wield_type_defender)
{
    std::shared_ptr<Item> weapon_ptr = attacker->equipment(weapon);
    //const std::shared_ptr<Item> ammo_ptr = (weapon_ptr->tag(EntityTag::WeaponRanged) ? attacker->inv()->at(attacker->ammo_pos(weapon_ptr)) : nullptr);
    const bool attacker_is_player = attacker->type() == EntityType::PLAYER;
    const bool defender_is_player = defender->type() == EntityType::PLAYER;
    const bool player_can_see_attacker = (attacker_is_player || attacker->is_in_fov());
    const bool player_can_see_defender = (defender_is_player || defender->is_in_fov());

    const std::string attacker_name = (attacker_is_player ? "you" : (player_can_see_attacker ? attacker->name(NAME_FLAG_THE) : "something"));
    const std::string defender_name = (defender_is_player ? "you" : (player_can_see_defender ? defender->name(NAME_FLAG_THE) : "something"));
    const std::string defender_name_c = StrX::capitalize_first_letter(defender_name);
    const std::string defender_name_s = (defender_is_player ? "your" : StrX::possessive_string(defender_name));
    const std::string attacker_your_string = (attacker_is_player ? "your" : StrX::possessive_string(attacker_name));
    const std::string attacker_your_string_c = StrX::capitalize_first_letter(attacker_your_string);
    //const std::string weapon_name = (ammo_ptr ? ammo_ptr->name() : weapon_ptr->name());
    std::string weapon_name = weapon_ptr->name();

    // For combat message colours.
    std::string bad_colour_attacker = "{u}", bad_colour_defender = "{u}", good_colour_attacker = "{u}", good_colour_defender = "{u}";
    if (attacker_is_player)
    {
        good_colour_attacker = "{g}";
        good_colour_defender = "{y}";
        bad_colour_attacker = "{y}";
        bad_colour_defender = "{c}";
    }
    else if (defender_is_player)
    {
        good_colour_attacker = "{r}";
        good_colour_defender = "{c}";
        bad_colour_attacker = "{c}";
        bad_colour_defender = "{r}";
    }

    // Determine what weapon skills the attacker and defender are using.
    int hit_bonus = 0, damage_bonus = 0;
    const bool ranged_attack = (wield_type_attacker == WieldType::RANGED_ATTACK);
    const bool finesse_weapon = (weapon_ptr->tag(EntityTag::WeaponFinesse));

    const int might_hit_bonus = (attacker->might() * MIGHT_HIT_BONUS);
    const int might_damage_bonus = (attacker->might() * MIGHT_DAMAGE_BONUS);
    const int finesse_hit_bonus = (attacker->finesse() * FINESSE_HIT_BONUS);
    const int finesse_damage_bonus = (attacker->finesse() * FINESSE_DAMAGE_BONUS);
    if (ranged_attack)
    {
        hit_bonus = finesse_hit_bonus;
        damage_bonus = finesse_damage_bonus;
    }
    else if (finesse_weapon)
    {
        hit_bonus = std::max(finesse_hit_bonus, might_hit_bonus);
        damage_bonus = std::max(finesse_damage_bonus, might_damage_bonus);
    }
    else
    {
        hit_bonus = might_hit_bonus;
        damage_bonus = might_damage_bonus;
    }

    // Special to-hit modifiers for certain weapon styles.
    if (wield_type_attacker == WieldType::UNARMED)
    {
        weapon_ptr = CodexItem::generate(ItemID::UNARMED_ATTACK);
        weapon_name = weapon_ptr->name();
        damage_bonus = hit_bonus = 0;
    }
    else if (wield_type_attacker == WieldType::DUAL_WIELD)
    {
        if (weapon_ptr->tag(EntityTag::WeaponLight)) hit_bonus += DUAL_WIELD_TO_HIT_MOD_LIGHT;
        else if (weapon_ptr->tag(EntityTag::WeaponFinesse)) hit_bonus += DUAL_WIELD_TO_HIT_MOD_FINESSE;
        else hit_bonus += DUAL_WIELD_TO_HIT_MOD;
    }
    else if (wield_type_attacker == WieldType::SINGLE_WIELD)
    {
        if (weapon_ptr->tag(EntityTag::WeaponFinesse)) hit_bonus += SINGLE_WIELD_HIT_MOD_FINESSE;
        else hit_bonus += SINGLE_WIELD_HIT_MOD;
    }

    // Roll to hit!
    const int raw_hit_roll = Random::rng(20);
    const int hit_roll = raw_hit_roll + hit_bonus;

    // Check if the defender can attempt to block or parry.
    const bool can_block = (defender->is_awake() && ((wield_type_defender == WieldType::ONE_HAND_PLUS_SHIELD || wield_type_defender == WieldType::SHIELD_ONLY)
        && !defender->tag(EntityTag::CannotBlock)));
    const bool can_parry = (!ranged_attack && defender->is_awake() && (wield_type_defender != WieldType::UNARMED && wield_type_defender !=
        WieldType::SHIELD_ONLY && !defender->tag(EntityTag::CannotParry)));

    // Defenders that cannot dodge, or are not awake, always get hit.
    // Ranged attacks always hit; their accuracy is determined by the actual path of the projectile.
    const bool guaranteed_hit = (defender->tag(EntityTag::CannotDodge) || !defender->is_awake() || ranged_attack);

    bool parried = false, blocked = false, evaded = false;
    const int defender_armour = defender->armour();
    const int defender_dodge = defender->dodge();

    if (DEBUG_VIEW_COMBAT_NUMBERS)
        core()->message("{w}Hit roll: {c}" + std::to_string(raw_hit_roll) + " {w}+ {c}" + std::to_string(hit_bonus) + " {w}({c}" +
            std::to_string(raw_hit_roll + hit_bonus) + "{w}), defender armour: {c}" + std::to_string(defender_armour) + "{w}, defender dodge: {c}" +
            std::to_string(defender_dodge));

    if (guaranteed_hit || hit_roll >= std::max(defender_armour, defender_dodge))    // Evasion failed; the target was hit.
    {
        // Now to check if the defender can successfully parry this attack.
        if (can_parry)
        {
            // Parrying is not yet implemented.
            parried = false;
        }

        // If evasion and parry both fail, we can try to block. Parrying is 'better' than blocking (parrying negates damage entirely) so there's no reason to
        // run a block check after a successful parry.
        if (!parried && can_block)
        {
            auto shield_item = defender->equipment(EquipSlot::HAND_OFF);
            if (shield_item->item_type() == ItemType::SHIELD && shield_item->armour() >= static_cast<int>(Random::rng(20))) blocked = true;
        }
    }
    else evaded = true;

    if (parried || evaded)
    {
        if (parried)
        {
            if (player_can_see_attacker || player_can_see_defender)
            {
                if (defender_is_player) core()->message(good_colour_defender + "You parry the " + attacker_your_string + " " + weapon_name + "!");
                else core()->message(good_colour_defender + attacker_your_string_c + " " + weapon_name + " is parried by " + defender_name + ".");
            }
        }
        else
        {
            if (player_can_see_attacker || player_can_see_defender)
            {
                auto defender_armour_item = defender->equipment(EquipSlot::BODY);
                bool dodged = (defender_armour_item->item_type() == ItemType::NONE);
                if ((defender_armour > defender_dodge && hit_roll < defender_dodge) || defender_dodge > defender_armour) dodged = true;
                if (dodged) core()->message(good_colour_defender + attacker_your_string_c + " " + weapon_name + " misses " + defender_name + ".");
                else core()->message(good_colour_defender + attacker_your_string_c + " " + weapon_name + " is deflected by " + defender_name_s +
                    defender->equipment(EquipSlot::BODY)->name() + ".");
            }
        }
    }
    else
    {
        defender->wake();   // Being attacked would wake anyone up.
        int damage = weapon_ptr->damage_roll() + damage_bonus;
        //if (wield_type_attacker == WieldType::RANGED_ATTACK) damage *= ammo_ptr->power();
        bool critical_hit = (raw_hit_roll == 20), bleed = false, poison = false;   // todo: add bleed and poison chance to weapons.
        if (wield_type_attacker == WieldType::SINGLE_WIELD) critical_hit = (raw_hit_roll >= 19);    // Single-wielding gives crits on 19-20.
        
        if (critical_hit)
        {
            bleed = true;
            damage *= CRITICAL_HIT_DAMAGE_MULTI;
        }

        if (damage < 0) damage = 0; // Just in case.

        if (defender->tag(EntityTag::ImmunityBleed)) bleed = false;
        if (defender->tag(EntityTag::ImmunityPoison)) poison = false;

        int damage_blocked = 0;
        if (blocked)
        {
            // blocking is not yet fully implemented
            damage_blocked = 0;
        }
        if (damage_blocked >= damage) damage_blocked = damage;
        damage -= damage_blocked;

        if (player_can_see_attacker || player_can_see_defender)
        {
            std::string damage_word = damage_str(damage, defender, false);
            std::string threshold_string = threshold_str(defender, damage, good_colour_attacker, bad_colour_defender);
            std::string damage_colour = good_colour_attacker;
            std::string death_str;
            int awaken_chance = AWAKEN_CHANCE_MOBS_FIGHTING_NEAR;
            if (damage >= defender->hp())
            {
                if (defender_is_player) death_str = " {m}You are slain!";
                else death_str = " {u}" + defender_name_c + (defender->tag(EntityTag::Unliving) ? " is destroyed!" : " is slain!");
                defender->set_tag(EntityTag::NoDeathMessage);
                awaken_chance = AWAKEN_CHANCE_MOB_DEATH_NEAR;
            }
            core()->message(damage_colour + attacker_your_string_c + " " + weapon_name + " " + damage_word + " " + damage_colour + defender_name + "! " +
                damage_number_str(damage, damage_blocked, critical_hit, bleed, poison) + threshold_string + death_str, awaken_chance);
            defender->take_damage(damage);
        }
    }
}

// Returns a threshold string, if a damage threshold has been passed.
std::string Combat::threshold_str(std::shared_ptr<Mobile> defender, int damage, const std::string &good_colour, const std::string &bad_colour)
{
    const bool alive = !defender->tag(EntityTag::Unliving);
    const bool is_player = defender->type() == EntityType::PLAYER;
    const bool plural = defender->tag(EntityTag::PluralName) || is_player;
    const std::string name = (is_player ? " You " : (plural ? " They " : " It "));

    const float old_perc = static_cast<float>(defender->hp()) / static_cast<float>(defender->hp(true));
    float new_perc = (static_cast<float>(defender->hp()) - static_cast<float>(damage)) / static_cast<float>(defender->hp(true));
    if (defender->hp() <= damage) new_perc = 0;

    if (old_perc >= 0.99f && new_perc >= 0.95f) return bad_colour + name + (alive ? (plural ? "barely notice." : "barely notices.") :
        (plural ? "are barely scratched." : "is barely scratched."));
    if (old_perc >= 0.99f && new_perc >= 0.95f) return bad_colour + name + (alive ? (plural ? "barely notice." : "barely notices.") :
        (plural ? "are barely scratched." : "is barely scratched."));
    if (old_perc >= 0.95f && new_perc >= 0.90f) return bad_colour + name + (alive ? (plural ? "shrug it off." : "shrugs it off.") :
        (plural ? "are hardly damaged." : "is hardly damaged."));
    if (old_perc >= 0.9f && new_perc == 0.0f) return good_colour + name + (plural ? "are utterly annihilated!" : "is utterly annihilated!");
    if (old_perc >= 0.9f && new_perc <= 0.2f) return good_colour + name + (plural ? "almost collapse!" : "almost collapses!");
    if (old_perc >= 0.9f && new_perc <= 0.4f) return good_colour + name + (plural ? "reel from the blow!" : "reels from the blow!");
    if (!new_perc) return "";
    if (old_perc > 0.1f && new_perc <= 0.1f) return good_colour + name + (alive ? (plural ? "are very close to death!" : "is very close to death!") :
            (plural ? "are very close to collapse!" : "is very close to collapse!"));
    if (old_perc > 0.2f && new_perc <= 0.2f) return good_colour + name + (alive ? (plural ? "look badly injured!" : "looks badly injured!") :
        (plural ? "look badly damaged!" : "looks badly damaged!"));
    if (old_perc > 0.5f && new_perc <= 0.5f) return good_colour + name + (alive ? (plural ? "have a few cuts and bruises." : "has a few cuts and bruises.") :
        (plural ? "have a few scratches and dents." : "has a few scratches and dents."));
    return "";
}

}   // namespace invictus
