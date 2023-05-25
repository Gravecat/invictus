// util/winx.cpp -- Windows-specific extension and utility functions.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifdef INVICTUS_TARGET_WINDOWS

#include <cstdlib>
#include <windows.h>
#include <psapi.h>  // MUST be included AFTER windows.h

#include "util/winx.hpp"


namespace invictus
{

// Used internally with bring_to_foreground()
WINBOOL CALLBACK WinX::btf_callback(HWND window_handle, LPARAM pid)
{
    DWORD window_pid;
    GetWindowThreadProcessId(window_handle, &window_pid);   // Get the process ID of the window we just found.
    if (static_cast<DWORD>(pid) == window_pid)  // Is this the process we care about?
    {
        // Make sure the window isn't minimized.
        WINDOWPLACEMENT placement;
        memset(&placement, 0, sizeof(WINDOWPLACEMENT));
        placement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(window_handle, &placement);
        switch (placement.showCmd)
        {
            case SW_SHOWMAXIMIZED: ShowWindow(window_handle, SW_SHOWMAXIMIZED); break;
            case SW_SHOWMINIMIZED: ShowWindow(window_handle, SW_RESTORE); break;
            default: ShowWindow(window_handle, SW_NORMAL); break;
        }

        // Bring the found window to the foreground.
        SetForegroundWindow(window_handle);

        return false;   // Stop enumerating windows.
    }
    return true;    // Continue enumerating.
}

// Brings the invictus.exe window to the foreground if possible.
void WinX::bring_to_foreground(DWORD pid) { EnumWindows(&WinX::btf_callback, static_cast<LPARAM>(pid)); }

// Checks if invictus.exe is already running; if so, brings that window to the foreground then exits quietly.
void WinX::check_if_already_running()
{
    CreateMutexA(0, false, "Local\\Invictus");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        std::string process_name = pid_to_name(GetCurrentProcessId());
        process_name = process_name.substr(process_name.find_last_of("\\") + 1);
        if (process_name.size()) bring_to_foreground(find_process_id(process_name.c_str()));
        exit(EXIT_FAILURE);
    }
}

// Finds a given process ID, given a specified executable filename.
DWORD WinX::find_process_id(std::string filename)
{
    LPDWORD process_ids = static_cast<LPDWORD>(HeapAlloc(GetProcessHeap(), 0, WINX_FP_MAX_PROCESSES * sizeof(DWORD)));
    DWORD found_id = 0;
    if (process_ids)
    {
        DWORD cdw_processes = 0;
        if (EnumProcesses(process_ids, WINX_FP_MAX_PROCESSES * sizeof(DWORD), &cdw_processes))
        {
            LPTSTR base_name = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), 0, MAX_PATH * sizeof(TCHAR)));
            if (base_name)
            {
                cdw_processes /= sizeof(DWORD);
                for (DWORD i = 0; i < cdw_processes; i++)
                {
                    HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_ids[i]);
                    if (process_handle)
                    {
                        if (GetModuleBaseName(process_handle, NULL, base_name, MAX_PATH) > 0)
                        {
                            if (!lstrcmpi(base_name, filename.c_str()))
                            {
                                found_id = process_ids[i];
                                CloseHandle(process_handle);
                                break;
                            }
                        }
                        CloseHandle(process_handle);
                    }
                }
                HeapFree(GetProcessHeap(), 0, static_cast<LPVOID>(base_name));
            }
        }
        HeapFree(GetProcessHeap(), 0, static_cast<LPVOID>(process_ids));
    }
    return found_id;
}

// Takes a Windows process ID and returns the name of the process.
std::string WinX::pid_to_name(DWORD pid)
{
    std::string result;
    HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
    if (handle)
    {
        DWORD buffer_size = WINX_P2N_BUFFER_SIZE;
        char buffer[WINX_P2N_BUFFER_SIZE];
        if (QueryFullProcessImageNameA(handle, 0, buffer, &buffer_size)) result = buffer;
        CloseHandle(handle);
    }
    return result;
}

}       // namespace invictus
#endif  // INVICTUS_TARGET_WINDOWS
