// core/save-load.hpp -- Handles saving and loading the game state to/from disk.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef CORE_SAVE_LOAD_HPP_
#define CORE_SAVE_LOAD_HPP_

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>


namespace invictus
{

class Area;     // defined in area/area.hpp
class Entity;   // defined in entity/entity.hpp
class Item;     // defined in entity/item.hpp
class Mobile;   // defined in entity/mobile.hpp
class Monster;  // defined in entity/monster.hpp
class Player;   // defined in entity/player.hpp
class Tile;     // defined in area/tile.hpp

class SaveLoad
{
public:
    static std::shared_ptr<Area> load_area_from_file(const std::string &filename);  // Loads an Area from a specified file.
    static void load_game(const std::string &save_folder);  // Loads the game state from a specified folder.
    static void save_area_to_file(const std::string &filename, std::shared_ptr<Area> area); // Saves an Area to a specific file.
    static void save_game();    // Saves the game to a specified file.

private:
    enum class SaveTag : uint32_t { HEADER_A = 0x49564E49, HEADER_B = 0x53555443, SAVE_EOF = 0xCAFEB0BA, GAME_MANAGER = 1, ENTITY, INVENTORY, ITEM,
        MOBILE, PLAYER, AREA, ENTITIES, TILE_MEMORY, TILES, UI, MSGLOG, COMPRESSED_BLOB, COMPRESSED_BLOB_END, MONSTER, BUFFS };

    static void     check_tag(std::ifstream &save_file, SaveTag expected_tag);  // Checks for an expected tag in the save file, and aborts if it isn't found.
    static void     incompatible(unsigned int error_a = 0, unsigned int error_b = 0);   // Aborts loading an incompatible save file.
    static std::shared_ptr<Area> load_area(std::ifstream &save_file);       // Loads an Area from disk.
    static void     load_blob_compressed(std::ifstream &save_file, char* blob, uint32_t blob_size); // Loads a block of memory from disk, decompressing it.
    static std::shared_ptr<Entity> load_entity(std::ifstream &save_file);   // Loads an Entity from disk.
    static std::string  load_game_manager(std::ifstream &save_file);    // Loads the GameManager class state.
    static void     load_item(std::ifstream &save_file, std::shared_ptr<Item> item);    // Loads an Item from disk.
    static void     load_mobile(std::ifstream &save_file, std::shared_ptr<Mobile> mob); // Loads a Mobile from disk.
    static void     load_monster(std::ifstream &save_file, std::shared_ptr<Monster> monster);   // Loads a Monster from disk.
    static void     load_msglog(std::ifstream &save_file);  // Loads the message log from disk.
    static void     load_player(std::ifstream &save_file, std::shared_ptr<Player> player);  // Loads a Player from disk.
    static std::string load_string(std::ifstream &save_file);   // Loads a string from the save game file.
    static Tile     load_tile(std::ifstream &save_file);    // Loads a Tile from the save game file.
    static void     load_ui(std::ifstream &save_file);      // Loads the UI elements from the save game file.
    static void     save_area(std::ofstream &save_file, std::shared_ptr<Area> area);            // Saves an Area to disk.
    static void     save_blob_compressed(std::ofstream &save_file, char* blob, uint32_t blob_size); // Saves a block of memory to disk, in a compressed form.
    static void     save_entity(std::ofstream &save_file, std::shared_ptr<Entity> entity);      // Saves an Entity to disk.
    static void     save_item(std::ofstream &save_file, std::shared_ptr<Item> item);            // Saves an Item to disk.
    static void     save_game_manager(std::ofstream &save_file);        // Saves the GameManager class state.
    static void     save_mobile(std::ofstream &save_file, std::shared_ptr<Mobile> mob);         // Saves a Mobile to disk.
    static void     save_monster(std::ofstream &save_file, std::shared_ptr<Monster> monster);   // Saves a Monster to disk.
    static void     save_msglog(std::ofstream &save_file);  // Saves the message log to disk.
    static void     save_player(std::ofstream &save_file, std::shared_ptr<Player> player);  // Saves a Player to disk.
    static void     save_string(std::ofstream &save_file, const std::string &str);  // Saves a string to the save game file.
    static void     save_ui(std::ofstream &save_file);  // Saves the UI elements to the save game file.
    static void     save_tile(std::ofstream &save_file, Tile tile);     // Saves a Tile to the save game file.
    static void     write_tag(std::ofstream &save_file, SaveTag tag);   // Writes a save tag to the save game file.

    // Loads simple data (ints, chars, floats, etc.) from the save file.
    template<class T> static T load_data(std::ifstream &save_file)
    { T data; save_file.read((char*)&data, sizeof(T)); return data; }

    // Saves simple data (ints, chars, floats, etc.) to the save file.
    template<class T> static void save_data(std::ofstream &save_file, T data)
    { save_file.write((char*)&data, sizeof(T)); }

    static const uint32_t   SAVE_VERSION =      17; // Increment this every time saved games are no longer compatible.
    static const uint32_t   SAVE_SUBVERSION =   0;  // The game is able to load saves of the same version, and any current or older subversion.

    static constexpr int    SAVE_ERROR_VERSION =    1;  // The save file version does not match.
    static constexpr int    SAVE_ERROR_ENTITY =     2;  // Something went wrong trying to load an Entity.
    static constexpr int    SAVE_ERROR_EQUIPMENT =  3;  // Equipment slot size mismatch.
    static constexpr int    SAVE_ERROR_BLOB =       4;  // Size mismatch when loading a compressed blob.
    static constexpr int    SAVE_ERROR_SUBVERSION = 5;  // The save file's subversion is newer than the running binary.
};



}       // namespace invictus
#endif  // CORE_SAVE_LOAD_HPP_
