// util/random.cpp -- Random number generation utility code, to make RNG a little easier.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <cstdlib>
#include <ctime>

#include "util/random.hpp"


namespace invictus
{

bool Random::seeded_ = false;   // Has the RNG been seeded yet?

// Generates a random number between, and including, the two specified values.
unsigned int Random::rng(unsigned int min, unsigned int max)
{
    if (min >= max) return min;
    if (!seeded_) seed();
    return min + (std::rand() % (max - min + 1));
}

// As above, but with an implied minimum number of 1.
unsigned int Random::rng(unsigned int max) { return rng(1, max); }

// 'Rolls' some virtual dice.
unsigned int Random::roll(unsigned int num_dice, unsigned int num_faces)
{
    if (!num_dice || !num_faces) return 0;
    unsigned int result = 0;

    for (unsigned int i = 0; i < num_dice; i++)
        result += rng(num_faces);

    return result;
}

// Seed the random number generator.
void Random::seed()
{
    std::srand((unsigned) std::time(NULL));
    seeded_ = true;
}


}   // namespace invictus
