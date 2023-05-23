# CMake build file for Morior Invictus, intended for use with MinGW/GCC on Windows and GCC on Linux.
# For MinGW/MSYS builds, be sure to specify -G"MinGW Makefiles" when setting up the build folder.
# Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

cmake_minimum_required(VERSION 3.13)


# Non-platform-specific stuff.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -pedantic-errors")
if(WARNINGS_AS_ERRORS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif(WARNINGS_AS_ERRORS)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_RELEASE  "-Os")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
add_definitions(-D__STDC_LIMIT_MACROS)
add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-DHAVE_CXX14)


# Main platform-specific settings.
if(TARGET_WINDOWS)

  # Platform-specific libraries.
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  set(OS_LIBRARIES mingw32)
  set(CURSES_LIBRARIES pdcurses)
  
  # Platform-specific linker flags. In this case, we're static-linking the MinGW C/C++ libraries and libwinpthread.
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,-Bdynamic,--no-whole-archive")

  # Application icon
  enable_language(RC)
  set(CMAKE_RC_COMPILER_INIT windres)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  set(INVICTUS_RC 3rdparty/app-icon/app-icon.rc)
  
elseif(TARGET_LINUX)

  # Platform-specific libraries.
  set(CURSES_LIBRARIES ncurses)

  # Platform-specific linker flags.
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-rpath='${ORIGIN}'")

elseif(TARGET_MACOS)

  message([FATAL_ERROR] "Morior Invictus is not currently supported on this platform.")

else()

  message([FATAL_ERROR] "Could not determine build platform!")

endif(TARGET_WINDOWS)


# Source files.
set(INVICTUS_CPPS
  core/core.cpp
  core/filex.cpp
  core/strx.cpp
  terminal/escape-code-index.cpp
  terminal/terminal.cpp
  terminal/window.cpp
)


# Binary file. INVICTUS_RC should be blank for non-Windows builds.
add_executable(invictus ${INVICTUS_CPPS} ${INVICTUS_RC})


# Include directories.
if(TARGET_WINDOWS)
  target_include_directories(invictus PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/3rdparty/pdcurses"
  )
else()
  target_include_directories(invictus PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
  )
endif()


# Platform-specific library directories.
if(TARGET_WINDOWS)
  if(TARGET_32BIT)
    target_link_directories(invictus PRIVATE "${CMAKE_SOURCE_DIR}/lib/win32")
  else()
    target_link_directories(invictus PRIVATE "${CMAKE_SOURCE_DIR}/lib/win64")
  endif(TARGET_32BIT)
endif(TARGET_WINDOWS)


# Link libraries. Platform-specific stuff should be set in the main platform-specific section near the top.
target_link_libraries(invictus
  ${OS_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CURSES_LIBRARIES}
)


# Post-build, make a 'bin' folder and copy the binary file in there.
add_custom_command(TARGET invictus POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:invictus> "${CMAKE_BINARY_DIR}/bin"
)
