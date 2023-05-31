// util/random.hpp -- Random number generation utility code, to make RNG a little easier.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UTIL_RANDOM_HPP_
#define UTIL_RANDOM_HPP_

namespace invictus
{

class Random
{
public:
    static unsigned int rng(unsigned int min, unsigned int max);    // Generates a random number between, and including, the two specified values.
    static unsigned int rng(unsigned int max);  // As above, but with an implied minimum number of 1.
    static float        rng_float(float min, float max);    // Retrieves a random float between the two specified values.
    static unsigned int roll(unsigned int num_dice, unsigned int num_faces);    // 'Rolls' some virtual dice.
    static void         seed();     // Seed the random number generator.

private:
    static bool     seeded_;    // Has the RNG been seeded yet?
};

}       // namespace invictus
#endif  // UTIL_RANDOM_HPP_
