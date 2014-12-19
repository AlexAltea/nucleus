/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_fp.h"
#include "nucleus/gpu/rsx_fp.h"

const char* getMask(u32 maskValue)
{
    static const char* maskString[] = {
        "",     //  0 -> 0000 [....]
        ".w",   //  1 -> 0001 [...w]
        ".z",   //  2 -> 0010 [..z.]
        ".zw",  //  3 -> 0011 [..zw]
        ".y",   //  4 -> 0100 [.y..]
        ".yw",  //  5 -> 0101 [.y.w]
        ".yz",  //  6 -> 0110 [.yz.]
        ".yzw", //  7 -> 0111 [.yzw]
        ".x",   //  8 -> 1000 [x...]
        ".xw",  //  9 -> 1001 [x..w]
        ".xz",  // 10 -> 1010 [x.z.]
        ".xzw", // 11 -> 1011 [x.zw]
        ".xy",  // 12 -> 1100 [xy..]
        ".xyw", // 13 -> 1101 [xy.w]
        ".xyz", // 14 -> 1110 [xyz.]
        ".xyzw" // 15 -> 1111 [xyzw]
    };
    return maskString[maskValue % 16];
}
