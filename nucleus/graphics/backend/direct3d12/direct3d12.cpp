/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12.h"
#include "nucleus/logger/logger.h"

// Load modules
#define LOAD_MODULE(module) \
    HMODULE hmodule_##module = LoadLibrary(#module ".dll"); \
    if (!hmodule_##module) { \
        logger.error(LOG_GRAPHICS, "initializeDirect3D12: Could not load " #module ".dll module"); \
        return false; \
    }

// Load functions
#define LOAD_FUNCTION(type, module, function) \
    _##function = reinterpret_cast<type>(GetProcAddress(hmodule_##module, #function)); \
    if (!_##function) { \
        logger.error(LOG_GRAPHICS, "Could not load Direct3D12 function '%s'", #function); \
        return false; \
    }

namespace gfx {
namespace direct3d12 {

// Declare functions
#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define DECLARE_FUNCTION(type, module, function) type _##function;
#define FUNCTION DECLARE_FUNCTION
#include "direct3d12.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION
#endif

bool initializeDirect3D12() {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    LOAD_MODULE(d3d12);
    LOAD_MODULE(dxgi);
    LOAD_MODULE(d3dcompiler_47);

#define FUNCTION LOAD_FUNCTION
#include "direct3d12.inl"
#undef FUNCTION
#endif
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
