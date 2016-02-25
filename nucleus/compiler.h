/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "platform.h"

#if defined(_MSC_VER)
#define NUCLEUS_COMPILER_MSVC
#endif
#if defined(__GNUC__)
#define NUCLEUS_COMPILER_GCC
#endif
#if defined(__clang__)
#define NUCLEUS_COMPILER_CLANG
#endif

// Glue: Error 'to_string' is not a member of 'std'. Under 'aarch64-linux-android-gcc.exe' (GCC 4.9)
#if defined(NUCLEUS_COMPILER_GCC) && defined(NUCLEUS_PLATFORM_ANDROID)
#include <string>
#include <sstream>
namespace std {
    template <typename T>
    std::string to_string(T value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }
}
#endif
