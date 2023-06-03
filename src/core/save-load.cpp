// core/save-load.cpp -- Handles saving and loading the game state to/from disk.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <algorithm>

#include "area/area.hpp"
#include "area/tile.hpp"
#include "codex/codex-tile.hpp"
#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "core/guru.hpp"
#include "core/save-load.hpp"
#include "entity/buff.hpp"
#include "entity/item.hpp"
#include "entity/monster.hpp"
#include "entity/player.hpp"
#include "ui/msglog.hpp"
#include "ui/ui.hpp"


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

// Loads an Area from disk.
std::shared_ptr<Area> SaveLoad::load_area(std::ifstream &save_file)
{
    check_tag(save_file, SaveTag::AREA);
    uint16_t size_x = load_data<uint16_t>(save_file);
    uint16_t size_y = load_data<uint16_t>(save_file);
    auto area = std::make_shared<Area>(size_x, size_y);
    area->offset_x_ = load_data<int>(save_file);
    area->offset_y_ = load_data<int>(save_file);
    area->file_ = load_string(save_file);
    area->level_ = load_data<int>(save_file);
    area->player_left_x_ = load_data<uint16_t>(save_file);
    area->player_left_y_ = load_data<uint16_t>(save_file);

    // Load the Entities in this Area.
    check_tag(save_file, SaveTag::ENTITIES);
    uint32_t entity_count = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < entity_count; i++)
        area->entities_.push_back(load_entity(save_file));

    // Load the tile memory.
    check_tag(save_file, SaveTag::TILE_MEMORY);
    load_blob_compressed(save_file, area->tile_memory_, size_x * size_y);

    // Load the individual tiles.
    check_tag(save_file, SaveTag::TILES);
    for (unsigned int i = 0; i < size_x * size_y; i++)
        area->tiles_[i] = load_tile(save_file);

    return area;
}

std::shared_ptr<Area> SaveLoad::load_area_from_file(const std::string &filename)
{
    std::ifstream area_file(filename, std::ios::in | std::ios::binary);
    check_tag(area_file, SaveTag::HEADER_A);
    check_tag(area_file, SaveTag::HEADER_B);
    uint32_t file_version = load_data<uint32_t>(area_file);
    uint32_t file_subversion = load_data<uint32_t>(area_file);
    if (file_version != SAVE_VERSION) incompatible(SAVE_ERROR_VERSION, file_version);
    else if (file_subversion > SAVE_SUBVERSION) incompatible(SAVE_ERROR_SUBVERSION, file_subversion);
    auto new_area = load_area(area_file);
    check_tag(area_file, SaveTag::SAVE_EOF);
    area_file.close();
    return new_area;
}

// Loads a block of memory from disk, decompressing it.
void SaveLoad::load_blob_compressed(std::ifstream &save_file, char* blob, uint32_t blob_size)
{
    check_tag(save_file, SaveTag::COMPRESSED_BLOB);
    uint32_t size_check = load_data<uint32_t>(save_file);
    if (size_check != blob_size) incompatible(SAVE_ERROR_BLOB, size_check);

    uint32_t count = 0;
    while (count < blob_size)
    {
        uint32_t seq_count = load_data<uint32_t>(save_file);
        char ch = load_data<char>(save_file);
        std::fill_n(blob + count, seq_count, ch);
        count += seq_count;
    }
    check_tag(save_file, SaveTag::COMPRESSED_BLOB_END);
}

// Loads an Entity from disk.
std::shared_ptr<Entity> SaveLoad::load_entity(std::ifstream &save_file)
{
    check_tag(save_file, SaveTag::ENTITY);

    // Determine the type of Entity to load.
    EntityType type = static_cast<EntityType>(load_data<uint8_t>(save_file));
    std::shared_ptr<Entity> entity = nullptr;
    switch(type)
    {
        case EntityType::PLAYER: entity = std::make_shared<Player>(); break;
        case EntityType::ITEM: entity = std::make_shared<Item>(); break;
        case EntityType::MONSTER: entity = std::make_shared<Monster>(); break;
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
        case EntityType::PLAYER:
            load_player(save_file, std::dynamic_pointer_cast<Player>(entity));
            load_mobile(save_file, std::dynamic_pointer_cast<Mobile>(entity));
            break;
        case EntityType::MONSTER:
            load_monster(save_file, std::dynamic_pointer_cast<Monster>(entity));
            load_mobile(save_file, std::dynamic_pointer_cast<Mobile>(entity));
            break;
        default: incompatible(SAVE_ERROR_ENTITY, static_cast<uint32_t>(type));
    }

    return entity;
}

// Loads the game state from a specified file.
void SaveLoad::load_game(const std::string &save_folder)
{
    auto game = core()->game();
    game->save_folder_ = save_folder;

    std::ifstream save_file(save_folder + "/game.dat", std::ios::in | std::ios::binary);
    if (!save_file.good()) throw std::runtime_error("Cannot find saved game file");
    check_tag(save_file, SaveTag::HEADER_A);
    check_tag(save_file, SaveTag::HEADER_B);
    uint32_t file_version = load_data<uint32_t>(save_file);
    uint32_t file_subversion = load_data<uint32_t>(save_file);
    if (file_version != SAVE_VERSION) incompatible(SAVE_ERROR_VERSION, file_version);
    else if (file_subversion > SAVE_SUBVERSION) incompatible(SAVE_ERROR_SUBVERSION, file_subversion);
    std::string area_filename = load_game_manager(save_file);
    game->player_ = std::dynamic_pointer_cast<Player>(load_entity(save_file));
    check_tag(save_file, SaveTag::SAVE_EOF);
    save_file.close();

    game->area_ = load_area_from_file(save_folder + "/" + area_filename + ".dat");
}

// Loads the GameManager class state.
std::string SaveLoad::load_game_manager(std::ifstream &save_file)
{
    auto game_manager = core()->game();

    check_tag(save_file, SaveTag::GAME_MANAGER);
    game_manager->game_state_ = static_cast<GameState>(load_data<uint8_t>(save_file));
    game_manager->heartbeat_ = load_data<float>(save_file);
    game_manager->heartbeat10_ = load_data<float>(save_file);
    std::string area_filename = load_string(save_file);
    load_ui(save_file);
    return area_filename;
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
    mob->hp_[0] = load_data<uint16_t>(save_file);
    mob->hp_[1] = load_data<uint16_t>(save_file);
    mob->mp_[0] = load_data<uint16_t>(save_file);
    mob->mp_[1] = load_data<uint16_t>(save_file);
    mob->sp_[0] = load_data<uint16_t>(save_file);
    mob->sp_[1] = load_data<uint16_t>(save_file);
    mob->bloody_feet_ = load_data<float>(save_file);
    mob->awake_ = load_data<bool>(save_file);
    mob->move_speed_ = load_data<float>(save_file);
    mob->regen_speed_[0] = load_data<float>(save_file);
    mob->regen_speed_[1] = load_data<float>(save_file);
    mob->regen_speed_[2] = load_data<float>(save_file);
    mob->regen_timer_[0] = load_data<float>(save_file);
    mob->regen_timer_[1] = load_data<float>(save_file);
    mob->regen_timer_[2] = load_data<float>(save_file);

    // Load the equipment.
    uint8_t equ_size = load_data<uint8_t>(save_file);
    if (equ_size != static_cast<uint8_t>(EquipSlot::_END)) incompatible(SAVE_ERROR_EQUIPMENT, equ_size);
    for (unsigned int i = 0; i < equ_size; i++)
    {
        uint8_t gear_exists = load_data<uint8_t>(save_file);
        if (gear_exists) mob->equipment_.at(i) = std::dynamic_pointer_cast<Item>(load_entity(save_file));
    }

    // Load the buffs, if any.
    check_tag(save_file, SaveTag::BUFFS);
    const uint32_t buff_size = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < buff_size; i++)
    {
        BuffType type = static_cast<BuffType>(load_data<uint16_t>(save_file));
        int power = load_data<int>(save_file);
        int time_left = load_data<int>(save_file);
        mob->buffs_.push_back(std::make_shared<Buff>(type, power, time_left));
    }
}

// Loads a Monster from disk.
void SaveLoad::load_monster(std::ifstream &save_file, std::shared_ptr<Monster> monster)
{
    check_tag(save_file, SaveTag::MONSTER);

    monster->banked_ticks_ = load_data<float>(save_file);
    monster->last_dir_ = load_data<uint8_t>(save_file);
    monster->player_last_seen_x_ = load_data<int>(save_file);
    monster->player_last_seen_y_ = load_data<int>(save_file);
    monster->dodge_ = load_data<uint8_t>(save_file);
    monster->to_damage_bonus_ = load_data<int8_t>(save_file);
    monster->to_hit_bonus_ = load_data<int8_t>(save_file);
    monster->tracking_turns_ = load_data<int16_t>(save_file);
}

// Loads the message log from disk.
void SaveLoad::load_msglog(std::ifstream &save_file)
{
    auto msglog = core()->game()->ui()->msglog();
    check_tag(save_file, SaveTag::MSGLOG);
    uint32_t size = load_data<uint32_t>(save_file);
    for (unsigned int i = 0; i < size; i++)
    {
        msglog->output_raw_.push_back(load_string(save_file));
        msglog->output_raw_fade_.push_back(load_data<uint8_t>(save_file));
    }
    msglog->output_raw_.push_back("{c}Game saved.");
    msglog->output_raw_fade_.push_back(false);
}

// Loads a Player from disk.
void SaveLoad::load_player(std::ifstream &save_file, std::shared_ptr<Player> player)
{
    check_tag(save_file, SaveTag::PLAYER);

    player->finesse_ = load_data<int8_t>(save_file);
    player->intellect_ = load_data<int8_t>(save_file);
    player->might_ = load_data<int8_t>(save_file);
    player->rest_time_ = load_data<int>(save_file);
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

// Loads a Tile from the save game file.
Tile SaveLoad::load_tile(std::ifstream &save_file)
{
    TileID tile_id = static_cast<TileID>(load_data<uint16_t>(save_file));
    uint8_t changed = load_data<uint8_t>(save_file);
    Tile new_tile;
    CodexTile::generate(&new_tile, tile_id);
    if (!changed) return new_tile;

    new_tile.ascii_ = load_data<char>(save_file);
    new_tile.ascii_scars_ = load_data<char>(save_file);
    new_tile.colour_ = static_cast<Colour>(load_data<uint8_t>(save_file));
    new_tile.colour_scars_ = static_cast<Colour>(load_data<uint8_t>(save_file));
    new_tile.name_ = load_string(save_file);

    // Load the TileTags.
    uint32_t tag_count = load_data<uint32_t>(save_file);
    new_tile.tags_.clear();
    for (unsigned int i = 0; i < tag_count; i++)
        new_tile.tags_.insert(static_cast<TileTag>(load_data<uint16_t>(save_file)));

    return new_tile;
}

// Loads the UI elements from the save game file.
void SaveLoad::load_ui(std::ifstream &save_file)
{
    check_tag(save_file, SaveTag::UI);
    load_msglog(save_file);
}

// Saves an Area to disk.
void SaveLoad::save_area(std::ofstream &save_file, std::shared_ptr<Area> area)
{
    write_tag(save_file, SaveTag::AREA);
    save_data<uint16_t>(save_file, area->size_x_);
    save_data<uint16_t>(save_file, area->size_y_);
    save_data<int>(save_file, area->offset_x_);
    save_data<int>(save_file, area->offset_y_);
    save_string(save_file, area->file_);
    save_data<int>(save_file, area->level_);
    save_data<uint16_t>(save_file, area->player_left_x_);
    save_data<uint16_t>(save_file, area->player_left_y_);

    // Save the Entities in this Area.
    write_tag(save_file, SaveTag::ENTITIES);
    save_data<uint32_t>(save_file, area->entities_.size() - 1);
    for (unsigned int i = 1; i < area->entities_.size(); i++)
        save_entity(save_file, area->entities_.at(i));

    // Save the tile memory. This could probably be compressed someday by storing the long sequences of spaces as some sort of integer tag, but not today.
    write_tag(save_file, SaveTag::TILE_MEMORY);
    save_blob_compressed(save_file, area->tile_memory_, area->size_x_ * area->size_y_);

    // Save the individual tiles.
    write_tag(save_file, SaveTag::TILES);
    for (unsigned int i = 0; i < area->size_x_ * area->size_y_; i++)
        save_tile(save_file, area->tiles_[i]);
}

// Saves an Area to a specific file.
void SaveLoad::save_area_to_file(const std::string &filename, std::shared_ptr<Area> area)
{
    std::ofstream area_file(filename, std::ios::out | std::ios::binary);
    write_tag(area_file, SaveTag::HEADER_A);
    write_tag(area_file, SaveTag::HEADER_B);
    save_data<uint32_t>(area_file, SAVE_VERSION);
    save_data<uint32_t>(area_file, SAVE_SUBVERSION);
    save_area(area_file, area);
    write_tag(area_file, SaveTag::SAVE_EOF);
    area_file.close();
}

// Saves a block of memory to disk, in a compressed form.
void SaveLoad::save_blob_compressed(std::ofstream &save_file, char* blob, uint32_t blob_size)
{
    write_tag(save_file, SaveTag::COMPRESSED_BLOB);
    save_data<uint32_t>(save_file, blob_size);
    uint32_t old_char = 999, char_count = 0;

    auto dump_data = [&save_file, &old_char, &char_count]()
    {
        if (old_char == 999) return;
        SaveLoad::save_data<uint32_t>(save_file, char_count);
        SaveLoad::save_data<char>(save_file, old_char);
        char_count = 0;
        old_char = 999;
    };

    for (unsigned int i = 0; i < blob_size; i++)
    {
        char ch = blob[i];
        if (static_cast<uint32_t>(ch) != old_char)
        {
            dump_data();
            old_char = ch;
        }
        char_count++;
    }
    dump_data();
    write_tag(save_file, SaveTag::COMPRESSED_BLOB_END);
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
        case EntityType::PLAYER:
            save_player(save_file, std::dynamic_pointer_cast<Player>(entity));
            save_mobile(save_file, std::dynamic_pointer_cast<Mobile>(entity));
            break;
        case EntityType::MONSTER:
            save_monster(save_file, std::dynamic_pointer_cast<Monster>(entity));
            save_mobile(save_file, std::dynamic_pointer_cast<Monster>(entity));
            break;
        default: core()->guru()->halt("Unknwon entity type!", static_cast<uint32_t>(entity->type()));
    }
}

// Saves the game to a specified file.
void SaveLoad::save_game()
{
    const std::string save_dir = core()->game()->save_folder();
    std::ofstream save_file(save_dir + "/game.dat", std::ios::out | std::ios::binary);
    write_tag(save_file, SaveTag::HEADER_A);
    write_tag(save_file, SaveTag::HEADER_B);
    save_data<uint32_t>(save_file, SAVE_VERSION);
    save_data<uint32_t>(save_file, SAVE_SUBVERSION);
    save_game_manager(save_file);
    save_entity(save_file, core()->game()->player());
    write_tag(save_file, SaveTag::SAVE_EOF);
    save_file.close();

    auto area = core()->game()->area();
    save_area_to_file(save_dir + "/" + area->filename() + ".dat", core()->game()->area());
    core()->message("{c}Game saved.");
}

// Saves the GameManager class state.
void SaveLoad::save_game_manager(std::ofstream &save_file)
{
    auto game_manager = core()->game();

    write_tag(save_file, SaveTag::GAME_MANAGER);
    save_data<uint8_t>(save_file, static_cast<uint8_t>(game_manager->game_state_));
    save_data<float>(save_file, game_manager->heartbeat_);
    save_data<float>(save_file, game_manager->heartbeat10_);
    save_string(save_file, game_manager->area_->file_ + std::to_string(game_manager->area_->level_));
    save_ui(save_file);
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
    save_data<uint16_t>(save_file, mob->hp_[0]);
    save_data<uint16_t>(save_file, mob->hp_[1]);
    save_data<uint16_t>(save_file, mob->mp_[0]);
    save_data<uint16_t>(save_file, mob->mp_[1]);
    save_data<uint16_t>(save_file, mob->sp_[0]);
    save_data<uint16_t>(save_file, mob->sp_[1]);
    save_data<float>(save_file, mob->bloody_feet_);
    save_data<bool>(save_file, mob->awake_);
    save_data<float>(save_file, mob->move_speed_);
    save_data<float>(save_file, mob->regen_speed_[0]);
    save_data<float>(save_file, mob->regen_speed_[1]);
    save_data<float>(save_file, mob->regen_speed_[2]);
    save_data<float>(save_file, mob->regen_timer_[0]);
    save_data<float>(save_file, mob->regen_timer_[1]);
    save_data<float>(save_file, mob->regen_timer_[2]);

    // Saves the equipment.
    save_data<uint8_t>(save_file, static_cast<uint8_t>(EquipSlot::_END));
    for (auto eq : mob->equipment_)
    {
        if (eq->item_type_ == ItemType::NONE) save_data<uint8_t>(save_file, 0);
        else
        {
            save_data<uint8_t>(save_file, 1);
            save_entity(save_file, eq);
        }
    }

    // Save the buffs, if any.
    write_tag(save_file, SaveTag::BUFFS);
    save_data<uint32_t>(save_file, mob->buffs_.size());
    for (auto buff : mob->buffs_)
    {
        save_data<uint16_t>(save_file, static_cast<uint16_t>(buff->get_type()));
        save_data<int>(save_file, buff->get_power());
        save_data<int>(save_file, buff->get_time_left());
    }
}

// Saves a Monster to disk.
void SaveLoad::save_monster(std::ofstream &save_file, std::shared_ptr<Monster> monster)
{
    write_tag(save_file, SaveTag::MONSTER);

    save_data<float>(save_file, monster->banked_ticks_);
    save_data<uint8_t>(save_file, monster->last_dir_);
    save_data<int>(save_file, monster->player_last_seen_x_);
    save_data<int>(save_file, monster->player_last_seen_y_);
    save_data<uint8_t>(save_file, monster->dodge_);
    save_data<int8_t>(save_file, monster->to_damage_bonus_);
    save_data<int8_t>(save_file, monster->to_hit_bonus_);
    save_data<int16_t>(save_file, monster->tracking_turns_);
}

// Saves the message log to disk.
void SaveLoad::save_msglog(std::ofstream &save_file)
{
    auto msglog = core()->game()->ui()->msglog();
    write_tag(save_file, SaveTag::MSGLOG);
    save_data<uint32_t>(save_file, msglog->output_raw_.size());
    for (unsigned int i = 0; i < msglog->output_raw_.size(); i++)
    {
        save_string(save_file, msglog->output_raw_.at(i));
        save_data<uint8_t>(save_file, msglog->output_raw_fade_.at(i) ? 1 : 0);
    }
}

void SaveLoad::save_player(std::ofstream &save_file, std::shared_ptr<Player> player)
{
    write_tag(save_file, SaveTag::PLAYER);

    save_data<int8_t>(save_file, player->finesse_);
    save_data<int8_t>(save_file, player->intellect_);
    save_data<int8_t>(save_file, player->might_);
    save_data<int>(save_file, player->rest_time_);
}

// Saves a string to the save game file.
void SaveLoad::save_string(std::ofstream &save_file, const std::string &str)
{
    save_data<uint32_t>(save_file, str.size());
    save_file.write(str.c_str(), str.size());
}

// Saves a Tile to the save game file.
void SaveLoad::save_tile(std::ofstream &save_file, Tile tile)
{
    save_data<uint16_t>(save_file, static_cast<uint16_t>(tile.id_));
    uint8_t changed = (tile.tag(TileTag::Changed) ? 1 : 0);
    save_data<uint8_t>(save_file, changed);
    if (!changed) return;

    save_data<char>(save_file, tile.ascii_);
    save_data<char>(save_file, tile.ascii_scars_);
    save_data<uint8_t>(save_file, static_cast<uint8_t>(tile.colour_));
    save_data<uint8_t>(save_file, static_cast<uint8_t>(tile.colour_scars_));
    save_string(save_file, tile.name_);

    // Save the TileTags.
    save_data<uint32_t>(save_file, tile.tags_.size());
    for (auto tag : tile.tags_)
        save_data<uint16_t>(save_file, static_cast<uint16_t>(tag));
}

// Saves the UI elements to the save game file.
void SaveLoad::save_ui(std::ofstream &save_file)
{
    write_tag(save_file, SaveTag::UI);
    save_msglog(save_file);
}

// Writes a save tag to the save game file.
void SaveLoad::write_tag(std::ofstream &save_file, SaveTag tag)
{ save_data<uint32_t>(save_file, static_cast<uint32_t>(tag)); }

}   // namespace invictus
