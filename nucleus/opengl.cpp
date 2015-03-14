/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl.h"
#include "nucleus/emulator.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define LOAD_EXTENSION(type, function) { \
    function = reinterpret_cast<type>(wglGetProcAddress(#function)); \
    if (!function) { \
        nucleus.log.error(LOG_HLE, "Could not load OpenGL extension '%s'", #function); \
        return false; \
    } \
}
#endif

// Declare extensions
#define DECLARE_EXTENSION(type, function) type function;
#define EXTENSION DECLARE_EXTENSION
#include "opengl_tables.h"
#undef EXTENSION
#undef DECLARE_EXTENSION

bool openglInit()
{
#define EXTENSION LOAD_EXTENSION
#include "opengl_tables.h"
#undef EXTENSION

    return true;
}
