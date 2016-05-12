/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <cstdio>
#include <string>

#ifdef NUCLEUS_COMPILER_MSVC
#define snprintf _snprintf_s
#endif

#ifdef NUCLEUS_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#endif

// Equivalent of snprintf returning std::string objects
template <typename... Args>
std::string format(const char* pattern, Args... args)
{
    char buffer[64_KB];
    snprintf(buffer, sizeof(buffer), pattern, std::forward<Args>(args)...);
    return std::string(buffer);
}

// E.g.: Transform a "0777" hexadicimal string ([0-9A-Fa-f]+) into the unsigned numerical value 1911 (up to 64-bits)
U64 hexToNumber(const char* hex);

// E.g.: Transform a "1A2B" hexadicimal string ([0-9A-F]+) into a byte array "\x1A\x2B" (provided a zeroed buffer)
void hexToBytes(const char* hex, char* dst);

#ifdef NUCLEUS_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
