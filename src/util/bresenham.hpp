// util/bresenham.hpp -- Simple implementation of Bresenham's line-drawing algorithm.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_BRESENHAM_HPP_
#define UTIL_BRESENHAM_HPP_

#include <utility>


namespace invictus
{

class BresenhamLine
{
public:
    BresenhamLine(int x1, int y1, int x2, int y2);  // Sets up for line calculation.
    std::pair<int, int> step(); // Makes one step along the line.

private:
    int     delta_x_, delta_y_;
    int     sign_x_, sign_y_;
    int     param_;
    bool    swapped_;
    int     x_, x1_, x2_, y_, y1_, y2_;
};

}       // namespace invictus
#endif  // UTIL_BRESENHAM_HPP_
