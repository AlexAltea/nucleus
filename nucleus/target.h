/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Temporarily enable all targets
#define _NUCLEUS_TARGET_ALL

#if defined(_NUCLEUS_TARGET_ALL) || defined(_NUCLEUS_TARGET_PS3)
#define NUCLEUS_TARGET_PS3
#endif
#if defined(_NUCLEUS_TARGET_ALL) || defined(_NUCLEUS_TARGET_PS4)
#define NUCLEUS_TARGET_PS4
#endif
#if defined(_NUCLEUS_TARGET_ALL) || defined(_NUCLEUS_TARGET_PSVITA)
#define NUCLEUS_TARGET_PSVITA
#endif

namespace core {

enum Target {
    TARGET_PS3,
    TARGET_PS4,
    TARGET_PSVITA,
};

}  // namespace core
