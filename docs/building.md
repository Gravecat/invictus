# BUILD INSTRUCTIONS

## Windows

**Requirements:** [CMake](https://cmake.org/) version 3.13 or newer *(3.14 or newer strongly recommended)*, MinGW ([MinGW-w64](https://github.com/niXman/mingw-builds-binaries/releases) recommended, [MSYS2](https://www.msys2.org/) should also work).

The build directory can be anywhere, but for the purposes of this example, create a folder called `build` within the project's root directory. From a command prompt or MinGW shell (*not* MSYS shell) within the `build` folder, run the following command:

**MinGW-w64:** `cmake -G"MinGW Makefiles" ..`

**MSYS2:** `cmake -G"MSYS Makefiles" ..`

Assuming there are no errors, run `mingw32-make -j` (or `make -j` for MSYS2) once the CMake configuration is complete. The binary should appear within the `bin` subfolder, along with its required DLLs and static data files.

## Linux

**Requirements:** [CMake](https://cmake.org/) version 3.13 or newer *(3.14 or newer strongly recommended)*, and the development package `libncurses5-dev` (or the equivalent package for your distro).

The build directory can be anywhere, but for the purposes of this example, use the `build` folder within the project's root directory. From within the `build` folder, run the following command: `cmake ..`

Assuming there are no errors, run `make -j` once the CMake configuration is complete. The binary should appear within the `bin` subfolder, along with its static data files.

## Other Platforms

macOS X and other platforms are unsupported and untested at the time of writing. If you are able to compile Morior Invictus on another target platform and require changes to the build environment, please open a pull request.
