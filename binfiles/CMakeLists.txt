# CMake build file for Morior Invictus, intended for use with MinGW/GCC on Windows and GCC on Linux.
# For MinGW/MSYS builds, be sure to specify -G"MinGW Makefiles" when setting up the build folder.
# Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

cmake_minimum_required(VERSION 3.0)


# Copy all the game's static data into the bin folder where the compiled binary lives.
add_custom_target(invictus_readme ALL
  DEPENDS invictus
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/binfiles/bin-readme.txt" "${CMAKE_BINARY_DIR}/bin/readme.txt"
)

# Windows needs the PDCurses DLL.
if(TARGET_WINDOWS)
  if(TARGET_32BIT)
    add_custom_target(invictus_dll ALL
      DEPENDS invictus
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/binfiles/win32dll/pdcurses.dll" "${CMAKE_BINARY_DIR}/bin"
    )
  else()
    add_custom_target(invictus_dll ALL
      DEPENDS invictus
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/binfiles/win64dll/pdcurses.dll" "${CMAKE_BINARY_DIR}/bin"
    )
  endif(TARGET_32BIT)
endif(TARGET_WINDOWS)
