// core/filex.cpp -- Various utility functions that deal with creating, deleting, and manipulating files.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdio>
#include <dirent.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#ifdef INVICTUS_TARGET_WINDOWS
#include <windows.h>
#endif

#include "core/filex.hpp"


// Deletes a specified file. Simple enough, but we'll keep this function around in case there's any platform-specific weirdness that needs to be worked in.
void FileX::delete_file(const std::string &filename) { unlink(filename.c_str()); }

// Check if a directory exists.
bool FileX::directory_exists(const std::string &dir)
{
    struct stat info;
    if (stat(dir.c_str(), &info) != 0) return false;
    if (info.st_mode & S_IFDIR) return true;
    return false;
}

// Checks if a file exists.
bool FileX::file_exists(const std::string &file)
{
    struct stat info;
    return (stat(file.c_str(), &info) == 0);
}

// Returns a list of files in a given directory.
std::vector<std::string> FileX::files_in_dir(const std::string &directory, bool recursive)
{
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> files;
    if (!(dir = opendir(directory.c_str()))) throw std::runtime_error("Could not open directory: " + directory);
    while ((ent = readdir(dir)))
    {
        std::string filename = std::string(ent->d_name);
        if (filename == "." || filename == "..") continue;
        struct stat s;
        if (stat((directory + "/" + filename).c_str(), &s) == 0)
        {
            if (s.st_mode & S_IFDIR)
            {
                if (recursive)
                {
                    std::vector<std::string> result = files_in_dir(directory + "/" + filename, true);
                    for (size_t i = 0; i < result.size(); i++)
                        result.at(i) = filename + "/" + result.at(i);
                    files.reserve(files.size() + result.size());
                    files.insert(files.end(), result.begin(), result.end());
                }
            }
            else if (s.st_mode & S_IFREG) files.push_back(filename);
        }
    }
    closedir(dir);
    return files;
}

// Checks if a file is read-only.
bool FileX::is_read_only(const std::string &file)
{
#ifdef INVICTUS_TARGET_WINDOWS
    DWORD attributes = GetFileAttributes(file.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) return (attributes & FILE_ATTRIBUTE_READONLY);
#elif INVICTUS_TARGET_LINUX
    if (access(file.c_str(), R_OK) == 0 && access(file.c_str(), W_OK) != 0) return true;
#endif
    return false;
}

// Makes a new directory, if it doesn't already exist.
void FileX::make_dir(const std::string &dir)
{
    if (directory_exists(dir)) return;

#ifdef INVICTUS_TARGET_WINDOWS
    mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0777);
#endif
}

// Renames a file. Seems simple, but this function exists for when inevitably some platform-specific fuckery arises.
void FileX::rename_file(const std::string &old_name, const std::string &new_name) { rename(old_name.c_str(), new_name.c_str()); }
