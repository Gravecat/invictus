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

class Entity;   // defined in entity/entity.hpp
class Item;     // defined in entity/item.hpp
class Mobile;   // defined in entity/mobile.hpp
class Player;   // defined in entity/player.hpp

class SaveLoad
{
public:
    static void load_game(const std::string &filename); // Loads the game state from a specified file.
    static void save_game(const std::string &filename); // Saves the game to a specified file.

private:
    enum class SaveTag : uint32_t { HEADER_A = 0x49564E49, HEADER_B = 0x53555443, SAVE_EOF = 0xCAFEB0BA, GAME_MANAGER = 1, ENTITY, INVENTORY, ITEM,
        MOBILE, PLAYER };

    static void check_tag(std::ifstream &save_file, SaveTag expected_tag);  // Checks for an expected tag in the save file, and aborts if it isn't found.
    static void incompatible(unsigned int error_a = 0, unsigned int error_b = 0);   // Aborts loading an incompatible save file.
    static std::shared_ptr<Entity> load_entity(std::ifstream &save_file);   // Loads an Entity from disk.
    static void load_game_manager(std::ifstream &save_file);    // Loads the GameManager class state.
    static void load_item(std::ifstream &save_file, std::shared_ptr<Item> item);    // Loads an Item from disk.
    static void load_mobile(std::ifstream &save_file, std::shared_ptr<Mobile> mob); // Loads a Mobile from disk.
    static void load_player(std::ifstream &save_file, std::shared_ptr<Mobile> mob); // Loads a Player from disk.
    static std::string load_string(std::ifstream &save_file);   // Loads a string from the save game file.
    static void save_entity(std::ofstream &save_file, std::shared_ptr<Entity> entity);  // Saves an Entity to disk.
    static void save_item(std::ofstream &save_file, std::shared_ptr<Item> item);        // Saves an Item to disk.
    static void save_game_manager(std::ofstream &save_file);        // Saves the GameManager class state.
    static void save_mobile(std::ofstream &save_file, std::shared_ptr<Mobile> mob);     // Saves a Mobile to disk.
    static void save_player(std::ofstream &save_file, std::shared_ptr<Player> player);  // Saves a Player to disk.
    static void save_string(std::ofstream &save_file, const std::string &str);  // Saves a string to the save game file.
    static void write_tag(std::ofstream &save_file, SaveTag tag);   // Writes a save tag to the save game file.

    // Loads simple data (ints, chars, floats, etc.) from the save file.
    template<class T> static T load_data(std::ifstream &save_file)
    { T data; save_file.read((char*)&data, sizeof(T)); return data; }

    // Saves simple data (ints, chars, floats, etc.) to the save file.
    template<class T> static void save_data(std::ofstream &save_file, T data)
    { save_file.write((char*)&data, sizeof(T)); }

    static const uint32_t   SAVE_VERSION =  1;   // Increment this every time saved games are no longer compatible.

    static constexpr int    SAVE_ERROR_VERSION =    1;  // The save file version does not match.
    static constexpr int    SAVE_ERROR_ENTITY =     2;  // Something went wrong trying to load an Entity.
    static constexpr int    SAVE_ERROR_EQUIPMENT =  3;  // Equipment slot size mismatch.
};



}       // namespace invictus
#endif  // CORE_SAVE_LOAD_HPP_
