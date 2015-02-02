/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "format.h"

#include <cstdlib>

u64 hexToNumber(const char* hex)
{
    char* hexEnd;
    return strtoull(hex, &hexEnd, 16);
}

void hexToBytes(const char* hex, char* dst)
{
    for (size_t i = 0; hex[i] != 0; i++) {
        const u8 value = (hex[i] < 0x3A) ? (hex[i] - 0x30) : (hex[i] - 0x37);
        dst[i/2] |= ((i % 2) == 0) ? (value << 4) : (value);
    }
}
