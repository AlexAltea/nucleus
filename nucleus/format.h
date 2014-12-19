/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <cstdio>
#include <string>

#ifdef NUCLEUS_MSVC
#define snprintf _snprintf_s
#endif

#ifdef NUCLEUS_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#endif

// Equivalent of snprintf returning std::string objects
template<typename... Args>
std::string format(const char* pattern, Args... args)
{
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), pattern, std::forward<Args>(args)...);
    return std::string(buffer);
}

#ifdef NUCLEUS_CLANG
#pragma clang diagnostic pop
#endif
