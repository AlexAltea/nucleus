/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_program.h"

const char* get_mask(u8 maskValue)
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
        ""      // 15 -> 1111 [xyzw]
    };
    return maskString[maskValue % 16];
}

std::string get_swizzling(u8 swizzleValue)
{
    static const char* swizzleString[] = {
        "x", "y", "z", "w"
    };

    // Check if swizzling is required (0x1B -> 0b00011011 -> [0,1,2,3] -> ".xyzw")
    if (swizzleValue == 0x1B) {
        return "";
    }

    std::string swizzling = ".";
    swizzling += swizzleString[(swizzleValue >> 6) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 4) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 2) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 0) & 0x3];
    return swizzling;
}
