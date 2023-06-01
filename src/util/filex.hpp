// util/filex.hpp -- Various utility functions that deal with creating, deleting, and manipulating files.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_FILEX_HPP_
#define UTIL_FILEX_HPP_

#include <string>
#include <vector>


namespace invictus
{

class FileX
{
public:
    static void delete_file(const std::string &filename);   // Deletes a specified file.
    static void delete_files_in_dir(const std::string &dir);    // Deletes all files in a directory. USE WITH CAUTION!!
    static bool directory_exists(const std::string &dir);   // Check if a directory exists.
    static bool file_exists(const std::string &file);       // Checks if a file exists.
    static std::vector<std::string> files_in_dir(const std::string &directory, bool recursive = false); // Returns a list of files in a given directory.
    static bool is_read_only(const std::string &file);      // Checks if a file is read-only.
    static void make_dir(const std::string &dir);           // Makes a new directory, if it doesn't already exist.
    static void rename_file(const std::string &old_name, const std::string &new_name);  // Renames a file.
};

}       // namespace invictus
#endif  // UTIL_FILEX_HPP_
