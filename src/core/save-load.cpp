// core/save-load.cpp -- Handles saving and loading the game state to/from disk.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <map>

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/save-load.hpp"
#include "entity/item.hpp"
#include "entity/player.hpp"


namespace invictus
{

// Checks for an expected tag in the save file, and aborts if it isn't found.
void SaveLoad::check_tag(std::ifstream &save_file, SaveTag expected_tag)
{
    SaveTag found_tag = load_data<SaveTag>(save_file);
    if (static_cast<uint32_t>(found_tag) != static_cast<uint32_t>(expected_tag))
    {
        std::string error_str = "Save file tag error";
        if (expected_tag == SaveTag::HEADER_A || expected_tag == SaveTag::HEADER_B) error_str = "Saved game file corrupted!";
        core()->guru()->halt(error_str, static_cast<uint32_t>(expected_tag), static_cast<uint32_t>(found_tag));
    }
}

// Aborts loading an incompatible save file.
void SaveLoad::incompatible(unsigned int error_a, unsigned int error_b)
{ core()->guru()->halt("Incompatible saved game", error_a, error_b); }

// Loads an Entity from disk.
std::shared_ptr<Entity> SaveLoad::load_entity(std::ifstream &save_file)
{
    check_tag(save_file, SaveTag::ENTITY);

    // Determine the type of Entity to load.
    EntityType type = static_cast<EntityType>(load_data<uint8_t>(save_file));
    std::shared_ptr<Entity> entity = nullptr;
    switch(type)
    {
        case EntityType::MOBILE: entity = std::make_shared<Mobile>(); break;
        case EntityType::PLAYER: entity = std::make_shared<Player>(); break;
        case EntityType::ITEM: entity = std::make_shared<Item>(); break;
        default: incompatible(SAVE_ERROR_ENTITY, static_cast<uint8_t>(type)); break;
    }

    // Load the basic data.
    entity->ascii_ = load_data<char>(save_file);
    entity->colour_ = static_cast<Colour>(load_data<uint8_t>(save_file));
    entity->name_ = load_string(save_file);
    entity->x_ = load_data<uint8_t>(save_file);
    entity->y_ = load_data<uint8_t>(save_file);

    // Load the EntityProps.
    uint32_t prop_f_count = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < prop_f_count; i++)
    {
        EntityProp key = static_cast<EntityProp>(load_data<uint16_t>(save_file));
        float data = load_data<float>(save_file);
        entity->entity_properties_f_.insert({key, data});
    }
    uint32_t prop_i_count = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < prop_i_count; i++)
    {
        EntityProp key = static_cast<EntityProp>(load_data<uint16_t>(save_file));
        int32_t data = load_data<int32_t>(save_file);
        entity->entity_properties_i_.insert({key, data});
    }

    // Load the EntityTags.
    uint32_t tag_count = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < tag_count; i++)
        entity->tags_.insert(static_cast<EntityTag>(load_data<uint16_t>(save_file)));

    // Load the inventory.
    check_tag(save_file, SaveTag::INVENTORY);
    uint32_t inv_size = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < inv_size; i++)
    {
        auto new_item = load_entity(save_file);
        entity->inventory_.push_back(std::dynamic_pointer_cast<Item>(new_item));
    }

    switch(type)
    {
        case EntityType::ITEM: load_item(save_file, std::dynamic_pointer_cast<Item>(entity)); break;
        case EntityType::PLAYER: load_player(save_file, std::dynamic_pointer_cast<Player>(entity)); [[fallthrough]];
        case EntityType::MOBILE: load_mobile(save_file, std::dynamic_pointer_cast<Mobile>(entity)); break;
        default: incompatible(SAVE_ERROR_ENTITY, static_cast<uint32_t>(type));
    }

    return entity;
}

// Loads the game state from a specified file.
void SaveLoad::load_game(const std::string &filename)
{
    std::ifstream save_file(filename, std::ios::in | std::ios::binary);

    check_tag(save_file, SaveTag::HEADER_A);
    check_tag(save_file, SaveTag::HEADER_B);
    uint32_t file_version = load_data<uint32_t>(save_file);
    if (file_version != SAVE_VERSION) incompatible(SAVE_ERROR_VERSION, file_version);
    load_game_manager(save_file);
    check_tag(save_file, SaveTag::SAVE_EOF);

    core()->guru()->halt("all looks good so far");
}

// Loads the GameManager class state.
void SaveLoad::load_game_manager(std::ifstream &save_file)
{
    auto game_manager = core()->game();

    check_tag(save_file, SaveTag::GAME_MANAGER);
    game_manager->game_state_ = static_cast<GameState>(load_data<uint8_t>(save_file));
    game_manager->heartbeat_ = load_data<float>(save_file);
    game_manager->heartbeat10_ = load_data<float>(save_file);
    game_manager->player_ = std::dynamic_pointer_cast<Player>(load_entity(save_file));
}

// Loads an Item from disk.
void SaveLoad::load_item(std::ifstream &save_file, std::shared_ptr<Item> item)
{
    check_tag(save_file, SaveTag::ITEM);
    item->item_type_ = static_cast<ItemType>(load_data<uint8_t>(save_file));
    item->item_subtype_ = static_cast<ItemSub>(load_data<uint8_t>(save_file));
    item->stack_ = load_data<uint16_t>(save_file);
}

// Loads a Mobile from disk.
void SaveLoad::load_mobile(std::ifstream &save_file, std::shared_ptr<Mobile> mob)
{
    check_tag(save_file, SaveTag::MOBILE);

    // Load the basic data.
    mob->banked_ticks_ = load_data<float>(save_file);
    mob->last_dir_ = load_data<uint8_t>(save_file);
    mob->hp_[0] = load_data<uint16_t>(save_file);
    mob->hp_[1] = load_data<uint16_t>(save_file);
    mob->mp_[0] = load_data<uint16_t>(save_file);
    mob->mp_[1] = load_data<uint16_t>(save_file);
    mob->sp_[0] = load_data<uint16_t>(save_file);
    mob->sp_[1] = load_data<uint16_t>(save_file);

    // Load the equipment.
    uint8_t equ_size = load_data<uint8_t>(save_file);
    if (equ_size != static_cast<uint8_t>(EquipSlot::_END)) incompatible(SAVE_ERROR_EQUIPMENT, equ_size);
    for (unsigned int i = 0; i < equ_size; i++)
    {
        uint8_t gear_exists = load_data<uint8_t>(save_file);
        if (gear_exists) load_item(save_file, mob->equipment_.at(i));
    }
}

// Loads a Player from disk.
void SaveLoad::load_player(std::ifstream &save_file, std::shared_ptr<Mobile>)
{
    check_tag(save_file, SaveTag::PLAYER);
}

// Loads a string from the save game file.
std::string SaveLoad::load_string(std::ifstream &save_file)
{
    uint32_t len = load_data<uint32_t>(save_file);
    char* data = new char[len];
    save_file.read(data, len);
    std::string result = std::string(data, len);
    delete[] data;
    return result;
}

// Saves an Entity to disk.
void SaveLoad::save_entity(std::ofstream &save_file, std::shared_ptr<Entity> entity)
{
    write_tag(save_file, SaveTag::ENTITY);

    // Basic data is easy to save.
    save_data<uint8_t>(save_file, static_cast<uint8_t>(entity->type()));
    save_data<char>(save_file, entity->ascii_);
    save_data<uint8_t>(save_file, static_cast<uint8_t>(entity->colour_));
    save_string(save_file, entity->name_);
    save_data<uint8_t>(save_file, entity->x_);
    save_data<uint8_t>(save_file, entity->y_);

    // Save the EntityProps.
    save_data<uint32_t>(save_file, entity->entity_properties_f_.size());
    for (auto prop_f : entity->entity_properties_f_)
    {
        save_data<uint16_t>(save_file, static_cast<uint16_t>(prop_f.first));
        save_data<float>(save_file, prop_f.second);
    }
    save_data<uint32_t>(save_file, entity->entity_properties_i_.size());
    for (auto prop_i : entity->entity_properties_i_)
    {
        save_data<uint16_t>(save_file, static_cast<uint16_t>(prop_i.first));
        save_data<int32_t>(save_file, prop_i.second);
    }

    // Save the EntityTags.
    save_data<uint32_t>(save_file, entity->tags_.size());
    for (auto tag : entity->tags_)
        save_data<uint16_t>(save_file, static_cast<uint16_t>(tag));

    // Save the inventory.
    write_tag(save_file, SaveTag::INVENTORY);
    save_data<uint32_t>(save_file, entity->inventory_.size());
    for (auto ie : entity->inventory_)
        save_entity(save_file, ie);

    switch(entity->type())
    {
        case EntityType::ITEM: save_item(save_file, std::dynamic_pointer_cast<Item>(entity)); break;
        case EntityType::PLAYER: save_player(save_file, std::dynamic_pointer_cast<Player>(entity)); [[fallthrough]];
        case EntityType::MOBILE: save_mobile(save_file, std::dynamic_pointer_cast<Mobile>(entity)); break;
        default: core()->guru()->halt("Unknwon entity type!", static_cast<uint32_t>(entity->type()));
    }
}

// Saves the game to a specified file.
void SaveLoad::save_game(const std::string &filename)
{
    std::ofstream save_file(filename, std::ios::out | std::ios::binary);
    write_tag(save_file, SaveTag::HEADER_A);
    write_tag(save_file, SaveTag::HEADER_B);
    save_data<uint32_t>(save_file, SAVE_VERSION);
    save_game_manager(save_file);
    write_tag(save_file, SaveTag::SAVE_EOF);
    save_file.close();
}

// Saves the GameManager class state.
void SaveLoad::save_game_manager(std::ofstream &save_file)
{
    auto game_manager = core()->game();

    write_tag(save_file, SaveTag::GAME_MANAGER);
    save_data<uint8_t>(save_file, static_cast<uint8_t>(game_manager->game_state_));
    save_data<float>(save_file, game_manager->heartbeat_);
    save_data<float>(save_file, game_manager->heartbeat10_);
    save_entity(save_file, game_manager->player_);
}

// Saves an Item to disk.
void SaveLoad::save_item(std::ofstream &save_file, std::shared_ptr<Item> item)
{
    write_tag(save_file, SaveTag::ITEM);
    save_data<uint8_t>(save_file, static_cast<uint8_t>(item->item_type_));
    save_data<uint8_t>(save_file, static_cast<uint8_t>(item->item_subtype_));
    save_data<uint16_t>(save_file, item->stack_);
}

// Saves a Mobile to disk.
void SaveLoad::save_mobile(std::ofstream &save_file, std::shared_ptr<Mobile> mob)
{
    write_tag(save_file, SaveTag::MOBILE);

    // Save the basic data.
    save_data<float>(save_file, mob->banked_ticks_);
    save_data<uint8_t>(save_file, mob->last_dir_);
    save_data<uint16_t>(save_file, mob->hp_[0]);
    save_data<uint16_t>(save_file, mob->hp_[1]);
    save_data<uint16_t>(save_file, mob->mp_[0]);
    save_data<uint16_t>(save_file, mob->mp_[1]);
    save_data<uint16_t>(save_file, mob->sp_[0]);
    save_data<uint16_t>(save_file, mob->sp_[1]);

    // Saves the equipment.
    save_data<uint8_t>(save_file, static_cast<uint8_t>(EquipSlot::_END));
    for (auto eq : mob->equipment_)
    {
        if (eq->item_type_ == ItemType::NONE) save_data<uint8_t>(save_file, 0);
        else save_item(save_file, eq);
    }
}

void SaveLoad::save_player(std::ofstream &save_file, std::shared_ptr<Player>)
{
    write_tag(save_file, SaveTag::PLAYER);
}

// Saves a string to the save game file.
void SaveLoad::save_string(std::ofstream &save_file, const std::string &str)
{
    save_data<uint32_t>(save_file, str.size());
    save_file.write(str.c_str(), str.size());
}

// Writes a save tag to the save game file.
void SaveLoad::write_tag(std::ofstream &save_file, SaveTag tag)
{ save_data<uint32_t>(save_file, static_cast<uint32_t>(tag)); }

}   // namespace invictus
