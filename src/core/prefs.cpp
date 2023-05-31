// core/prefs.cpp -- User-defined preferences, which can be set in userdata/prefs.txt
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <fstream>
#include <vector>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "core/prefs.hpp"
#include "util/filex.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Constructor, sets default values.
Prefs::Prefs(std::string filename) : filename_(filename), pathfind_euclidean_(true), use_colour_(true)
{
#ifdef INVICTUS_TARGET_WINDOWS
    acs_flags_ = 15;
#endif
#ifdef INVICTUS_TARGET_LINUX
    acs_flags_ = 11;
#endif
}

// Retrieves the ACS glyph usage flags.
uint8_t Prefs::acs_flags() const { return acs_flags_; }

// Loads user prefs from a file, if it exists.
void Prefs::load()
{
    auto guru = core()->guru();
    guru->log("Attempting to load user preferences.");
    if (!FileX::file_exists(filename_))
    {
        guru->log("No user preferences file detected. Using default options.");
        return;
    }
    std::ifstream prefs_file(filename_);
    if (!prefs_file.is_open())
    {
        guru->nonfatal("Could not open " + filename_, GURU_WARN);
        return;
    }
    std::string line, pref, pref_val;
    std::vector<std::string> pref_vec;
    while (std::getline(prefs_file, line))
    {
        if (!line.size()) continue;
        pref_vec = StrX::string_explode(line, ":");
        if (pref_vec.size() != 2) guru->nonfatal("Invalid line in " + filename_ + ": " + line, GURU_WARN);
        else
        {
            pref = StrX::str_tolower(pref_vec.at(0));
            pref_val = pref_vec.at(1);
            if (!pref.compare("acs_flags")) acs_flags_ = std::stoi(pref_val);
            else if (!pref.compare("pathfind_euclidean")) pathfind_euclidean_ = StrX::str_to_bool(pref_val);
            else if (!pref.compare("use_colour")) use_colour_ = StrX::str_to_bool(pref_val);
            else guru->nonfatal("Invalid line in " + filename_ + ": " + line, GURU_WARN);
        }
    }
    prefs_file.close();
}

// Is the pathfinding code using the Euclidean method (true) or the Manhattan method (false)?
bool Prefs::pathfind_euclidean() const { return pathfind_euclidean_; }

// Saves user prefs to a file.
void Prefs::save()
{
    std::ofstream save_file(filename_);
    save_file << "acs_flags:" << std::to_string(acs_flags_) << std::endl;
    save_file << "pathfind_euclidean:" << StrX::bool_to_str(pathfind_euclidean_) << std::endl;
    save_file << "use_colour:" << StrX::bool_to_str(use_colour_) << std::endl;
    save_file.close();
}

// Check if using colour is allowed.
bool Prefs::use_colour() const { return use_colour_; }

}   // namespace invictus
