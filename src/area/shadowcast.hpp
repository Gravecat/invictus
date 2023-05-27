// area/shadowcast.hpp -- Shadowcasting code, for calculating line-of-sight.
// Based on original code from here: https://www.roguebasin.com/index.php/C%2B%2B_shadowcasting_implementation
// Modified slightly by Raine "Gravecat" Simmons, 2023.

#ifndef AREA_SHADOW_CAST_HPP_
#define AREA_SHADOW_CAST_HPP_

namespace invictus
{

class Area; // defined in area/area.hpp


class Shadowcast
{
public:
    static void calc_fov(Area* area, unsigned int x, unsigned int y, unsigned int radius);

private:
    static void cast_light(Area* area, unsigned int x, unsigned int y, unsigned int radius, unsigned int row, float start_slope, float end_slope,
        unsigned int xx, unsigned int xy, unsigned int yx, unsigned int yy);

    static int  multipliers[4][8];
};

}       // namespace invictus
#endif  // AREA_SHADOW_CAST_HPP_
