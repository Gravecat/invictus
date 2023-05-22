# CMake build file for Morior Invictus, intended for use with MinGW/GCC on Windows and GCC on Linux.
# For MinGW/MSYS builds, be sure to specify -G"MinGW Makefiles" when setting up the build folder.
# Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

cmake_minimum_required(VERSION 3.13)
project(invictus)

# Automatically default to release builds if a type is not specified.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Build type was not specified, switching to 'Release'.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake.
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Platform-specific defaults/overrides
if(WIN32)
  set(TARGET_WINDOWS 1)
  add_definitions(-DINVICTUS_TARGET_WINDOWS)
elseif(APPLE)
  set(TARGET_MACOS 1)
  add_definitions(-DINVICTUS_TARGET_MACOS)
elseif(UNIX AND NOT APPLE)
  set(TARGET_LINUX 1)
  add_definitions(-DINVICTUS_TARGET_LINUX)
else()
  message([FATAL_ERROR] "Could not determine build platform!")
endif(WIN32)

# Detect 32-bit or 64-bit build
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
  set(TARGET_32BIT 1)
endif()

# Optionally, compile extra-strict with warnings as errors.
option(WARNINGS_AS_ERRORS "WARNINGS_AS_ERRORS" OFF)

add_subdirectory(src)
