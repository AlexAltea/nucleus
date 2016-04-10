/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Temporarily enable all platforms
#define _NUCLEUS_PLATFORM_ALL

#if defined(_NUCLEUS_PLATFORM_ALL) || defined(_NUCLEUS_PLATFORM_PS3)
#define NUCLEUS_PLATFORM_PS3
#endif
#if defined(_NUCLEUS_PLATFORM_ALL) || defined(_NUCLEUS_PLATFORM_PS4)
#define NUCLEUS_PLATFORM_PS4
#endif
#if defined(_NUCLEUS_PLATFORM_ALL) || defined(_NUCLEUS_PLATFORM_PSP)
#define NUCLEUS_PLATFORM_PSP
#endif
#if defined(_NUCLEUS_PLATFORM_ALL) || defined(_NUCLEUS_PLATFORM_PSVITA)
#define NUCLEUS_PLATFORM_PSVITA
#endif

namespace core {

enum Platform {
    PLATFORM_UNKNOWN = 0,

    PLATFORM_PS3,
    PLATFORM_PS4,
    PLATFORM_PSP,
    PLATFORM_PSVITA,
};

}  // namespace core
