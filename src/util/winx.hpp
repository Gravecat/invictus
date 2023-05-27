// util/winx.hpp -- Windows-specific extension and utility functions.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_WINX_HPP_
#define UTIL_WINX_HPP_
#ifdef INVICTUS_TARGET_WINDOWS

#include <string>
#include <windows.h>


namespace invictus
{

class WinX
{
public:
    static WINBOOL CALLBACK btf_callback(HWND window_handle, LPARAM pid);   // Used internally with bring_to_foreground()
    static void check_if_already_running(); // Checks if invictus.exe is already running; if so, brings that window to the foreground then exits quietly.

private:
    // DO NOT ADJUST THESE VALUES. Just leave them alone.
    static constexpr int    WINX_FP_MAX_PROCESSES = 1024;
    static constexpr int    WINX_P2N_BUFFER_SIZE =  1024;

    static void         bring_to_foreground(DWORD pid);         // Brings the invictus.exe window to the foreground if possible.
    static DWORD        find_process_id(std::string filename);  // Finds a given process ID, given a specified executable filename.
    static std::string  pid_to_name(DWORD pid);                 // Takes a Windows process ID and returns the name of the process.
};

}       // namespace invictus
#endif  // INVICTUS_TARGET_WINDOWS
#endif  // UTIL_WINX_HPP_
