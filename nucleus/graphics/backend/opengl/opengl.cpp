/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "opengl.h"
#include "nucleus/common.h"

namespace graphics {

// Load extensions
#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define LOAD_EXTENSION(type, function) { \
    function = reinterpret_cast<type>(wglGetProcAddress(#function)); \
    if (!function) { \
        logger.error(LOG_HLE, "Could not load OpenGL extension '%s'", #function); \
        return false; \
    } \
}
#endif

// Declare extensions
#define DECLARE_EXTENSION(type, function) type function;
#define EXTENSION DECLARE_EXTENSION
#include "opengl.inc"
#undef EXTENSION
#undef DECLARE_EXTENSION

bool initializeOpenGL()
{
#define EXTENSION LOAD_EXTENSION
#include "opengl.inc"
#undef EXTENSION

    return true;
}

}  // namespace graphics
