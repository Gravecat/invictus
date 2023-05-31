// area/gore.hpp -- Handles splashes of blood and other viscera from combat.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef AREA_GORE_HPP_
#define AREA_GORE_HPP_

namespace invictus
{

class Gore
{
public:
    static int  gore_level(int x, int y);               // Determine the gore level of a given tile.
    static void set_gore(int x, int y, int level);      // Sets a tile to a given gore level.
    static void splash(int x, int y, int intensity);    // Splashes blood and gore on a given tile.

private:
    static void do_splash(int x, int y);    // Performs a single splatter.
};

}       // namespace invictus
#endif  // AREA_GORE_HPP_
