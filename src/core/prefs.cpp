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
Prefs::Prefs(std::string filename) : filename_(filename), use_colour_(true) { }

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
        guru->nonfatal("Could not open " + filename_, Guru::GURU_WARN);
        return;
    }
    std::string line, pref, pref_val;
    std::vector<std::string> pref_vec;
    while (std::getline(prefs_file, line))
    {
        if (!line.size()) continue;
        pref_vec = StrX::string_explode(line, ":");
        if (pref_vec.size() != 2) guru->nonfatal("Invalid line in " + filename_ + ": " + line, Guru::GURU_WARN);
        else
        {
            pref = StrX::str_tolower(pref_vec.at(0));
            pref_val = pref_vec.at(1);
            if (!pref.compare("use_colour")) use_colour_ = StrX::str_to_bool(pref_val);
            else guru->nonfatal("Invalid line in " + filename_ + ": " + line, Guru::GURU_WARN);
        }
    }
    prefs_file.close();
}

// Saves user prefs to a file.
void Prefs::save()
{
    std::ofstream save_file(filename_);
    save_file << "use_colour:" << StrX::bool_to_str(use_colour_) << std::endl;
    save_file.close();
}

// Check if using colour is allowed.
bool Prefs::use_colour() const { return use_colour_; }

}   // namespace invictus
