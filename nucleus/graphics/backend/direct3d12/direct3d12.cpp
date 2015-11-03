/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12.h"
#include "nucleus/logger/logger.h"

// Load functions
#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define LOAD_FUNCTION(type, function) { \
    _##function = reinterpret_cast<type>(GetProcAddress(module, #function)); \
    if (!_##function) { \
        logger.error(LOG_GRAPHICS, "Could not load Direct3D12 function '%s'", #function); \
        return false; \
    } \
}
#endif

// Declare functions
#define DECLARE_FUNCTION(type, function) type _##function;
#define FUNCTION DECLARE_FUNCTION
#include "direct3d12.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION

namespace gfx {

bool initializeDirect3D12() {
    HMODULE module = LoadLibrary("d3d12.dll");
    if (!module) {
        logger.error(LOG_GRAPHICS, "initializeDirect3D12: Could not load d3d12.dll module");
        return false;
    }

#define FUNCTION LOAD_FUNCTION
#include "direct3d12.inl"
#undef FUNCTION

    return true;
}

}  // namespace gfx
