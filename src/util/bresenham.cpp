// util/bresenham.cpp -- Simple implementation of Bresenham's line-drawing algorithm.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cmath>

#include "util/bresenham.hpp"


namespace invictus
{

// Sets up for line calculation.
BresenhamLine::BresenhamLine(int x1, int y1, int x2, int y2) : delta_x_(x2 - x1), delta_y_(y2 - y1), swapped_(false), x_(x1), x1_(x1), x2_(x2), y_(y1), y1_(y1),
    y2_(y2)
{
    sign_x_ = (delta_x_ >= 0) ? 1 : -1;
    sign_y_ = (delta_y_ >= 0) ? 1 : -1;

    if (std::abs(delta_y_) > std::abs(delta_x_))
    {
        int temp = delta_y_;
        delta_y_ = delta_x_;
        delta_x_ = temp;
        swapped_ = true;
    }

    param_ = 2 * std::abs(delta_y_) - std::abs(delta_x_);
}

// Makes one step along the line.
std::pair<int, int> BresenhamLine::step()
{
    if (param_ < 0)
    {
        if (!swapped_) x_ += sign_x_;
        else y_ += sign_y_;
        param_ += 2 * std::abs(delta_y_);
    }
    else
    {
        x_ += sign_x_;
        y_ += sign_y_;
        param_ += 2 * std::abs(delta_y_) - 2 * std::abs(delta_x_);
    }
    return std::pair<int, int>(x_, y_);
}

}   // namespace invictus
