// tune/ascii-symbols.hpp -- Definitions for which ASCII symbols are used for which things in the game, allowing for easily changing them en masse if needed.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef TUNE_ASCII_SYMBOLS_HPP_
#define TUNE_ASCII_SYMBOLS_HPP_

namespace invictus
{

//   (32, space)
constexpr char  ASCII_NOTHING =         ' ';    // Used for representing... nothing.

// ! (33, exclamation mark)

// " (34, double quote)

// # (35, hash)
constexpr char  ASCII_GORE_6B =         '#';    // Ground/floor tiles covered in blood/gore.
constexpr char  ASCII_WALL =            '#';    // Walls.

// $ (36, dollar sign)

// % (37, percent)
constexpr char  ASCII_GORE_6A =         '%';    // Ground/floor tiles covered in blood/gore.
constexpr char  ASCII_TOMB =            '%';    // Tombs, coffins, etc.

// & (38, ampersand)
constexpr char  ASCII_STACK =           '&';    // A stack of multiple items.

// ' (39, single quote)
constexpr char  ASCII_DOOR_OPEN =       '\'';   // Open doors.
constexpr char  ASCII_GORE_3 =          '\'';   // Ground/floor tiles with a fair amount of blood/gore.

// ( (40, open bracket)

// ) (41, closed bracket)

// * (42, star)
constexpr char  ASCII_UNKNOWN =         '*';    // Used to represent undefined items.

// + (43, plus)
constexpr char  ASCII_DOOR_CLOSED =     '+';    // Closed doors.

// , (44, comma)
constexpr char  ASCII_GORE_2 =          ',';    // Ground/floor tiles with a little blood/gore.

// - (45, dash)

// . (46, full stop)
constexpr char  ASCII_GORE_1 =          '.';    // Ground/floor tiles with the lowest blood/gore level.
constexpr char  ASCII_GROUND =          '.';    // Ground/floor tiles.

// / (47, forward slash)

// 0 (48, zero)

// 1 (49, one)

// 2 (50, two)

// 3 (51, three)

// 4 (52, four)

// 5 (53, five)

// 6 (54, six)

// 7 (55, seven)

// 8 (56, eight)

// 9 (57, nine)

// : (58, colon)
constexpr char  ASCII_GORE_4 =          ':';    // Ground/floor tiles with a lot of blood/gore.
constexpr char  ASCII_STONE =           ':';    // Stones or rocks.

// ; (59, semicolon)
constexpr char  ASCII_GORE_5 =          ';';    // Ground/floor tiles with a whole lot of blood/gore.

// < (60, less than)
constexpr char  ASCII_STAIRS_UP =       '<';    // Stairs leading upward.

// = (61, equals)

// > (62, greater than)
constexpr char  ASCII_STAIRS_DOWN =     '>';    // Stairs leading downward.

// ? (63, question mark)

// @ (64, at sign)
constexpr char  ASCII_PLAYER =          '@';     // The player character.

// A (65, upper-case A)

// B (66, upper-case B)

// C (67, upper-case C)

// D (68, upper-case D)

// E (69, upper-case E)

// F (70, upper-case F)

// G (71, upper-case G)

// H (72, upper-case H)

// I (73, upper-case I)

// J (74, upper-case J)

// K (75, upper-case K)

// L (76, upper-case L)

// M (77, upper-case M)

// N (78, upper-case N)

// O (79, upper-case O)

// P (80, upper-case P)

// Q (81, upper-case Q)

// R (82, upper-case R)

// S (83, upper-case S)

// T (84, upper-case T)

// U (85, upper-case U)

// V (86, upper-case V)

// W (87, upper-case W)

// X (88, upper-case X)

// Y (89, upper-case Y)

// Z (90, upper-case Z)

// [ (91, open square bracket)
constexpr char  ASCII_ARMOUR =          '[';     // Armour worn on the body.

// \ (92, backslash)
constexpr char  ASCII_BLUNT_WEAPON =    '\\';   // Blunt weapons such as maces and clubs.

// ] (93, closed square bracket)

// ^ (94, caret)

// _ (95, underscore)

// ` (96, grave)

// a (97, lower-case A)

// b (98, lower-case B)

// c (99, lower-case C)

// d (100, lower-case D)

// e (101, lower-case E)

// f (102, lower-case F)

// g (103, lower-case G)

// h (104, lower-case H)

// i (105, lower-case I)

// j (106, lower-case J)

// k (107, lower-case K)

// l (108, lower-case L)

// m (109, lower-case M)

// n (110, lower-case N)

// o (111, lower-case O)

// p (112, lower-case P)

// q (113, lower-case Q)

// r (114, lower-case R)

// s (115, lower-case S)

// t (116, lower-case T)

// u (117, lower-case U)

// v (118, lower-case V)

// w (119, lower-case W)

// x (120, lower-case X)

// y (121, lower-case Y)

// z (122, lower-case Z)
constexpr char  ASCII_ZOMBIE =          'z';    // Zombies, ghouls, and similar forms of undead.

// { (123, open curly brace)

// | (124, vertical bar)
constexpr char  ASCII_EDGED_WEAPON =    '|';     // Edged weapons such as swords.

// } (125, closed curly brace)

// ~ (126, tilde)
constexpr char  ASCII_CORPSE =          '~';    // Corpses of slain Mobiles.

}       // namespace invictus
#endif  // TUNE_ASCII_SYMBOLS_HPP_
