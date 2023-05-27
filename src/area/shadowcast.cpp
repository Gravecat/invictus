// area/shadowcast.cpp -- Shadowcasting code, for calculating line-of-sight.
// Based on original code from here: https://www.roguebasin.com/index.php/C%2B%2B_shadowcasting_implementation
// Modified slightly by Raine "Gravecat" Simmons, 2023.

#include <cmath>

#include "area/area.hpp"
#include "area/shadowcast.hpp"


namespace invictus
{

int Shadowcast::multipliers[4][8] = {
    {1, 0, 0, -1, -1, 0, 0, 1},
    {0, 1, -1, 0, 0, -1, 1, 0},
    {0, 1, 1, 0, 0, -1, -1, 0},
    {1, 0, 0, 1, -1, 0, 0, -1}
};

void Shadowcast::calc_fov(Area* area, unsigned int x, unsigned int y, unsigned int radius)
{
    for (unsigned int i = 0; i < 8; i++)
        cast_light(area, x, y, radius, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
}

void Shadowcast::cast_light(Area* area, unsigned int x, unsigned int y, unsigned int radius, unsigned int row, float start_slope, float end_slope,
    unsigned int xx, unsigned int xy, unsigned int yx, unsigned int yy)
{
    if (start_slope < end_slope) return;
    float next_start_slope = start_slope;

    for (unsigned int i = row; i <= radius; i++)
    {
        bool blocked = false;
        for (int dx = -i, dy = -i; dx <= 0; dx++)
        {
            float l_slope = (dx - 0.5) / (dy + 0.5);
            float r_slope = (dx + 0.5) / (dy - 0.5);
            if (start_slope < r_slope) continue;
            else if (end_slope > l_slope) break;

            int sax = dx * xx + dy * xy;
            int say = dx * yx + dy * yy;
            if ((sax < 0 && static_cast<unsigned int>(std::abs(sax)) > x) || (say < 0 && static_cast<unsigned int>(std::abs(say)) > y)) continue;
            unsigned int ax = x + sax;
            unsigned int ay = y + say;
            if (ax >= static_cast<unsigned int>(area->width()) || ay >= static_cast<unsigned int>(area->height())) continue;

            unsigned int radius2 = radius * radius;
            if (static_cast<unsigned int>(dx * dx + dy * dy) < radius2) area->set_visible(ax, ay);

            if (blocked)
            {
                if (area->is_opaque(ax, ay))
                {
                    next_start_slope = r_slope;
                    continue;
                }
                else
                {
                    blocked = false;
                    start_slope = next_start_slope;
                }
            }
            else if (area->is_opaque(ax, ay))
            {
                blocked = true;
                next_start_slope = r_slope;
                cast_light(area, x, y, radius, i + 1, start_slope, l_slope, xx, xy, yx, yy);
            }
        }
        if (blocked) break;
    }
}

}   // namespace invictus
