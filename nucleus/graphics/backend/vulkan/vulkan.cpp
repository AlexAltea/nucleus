/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan.h"
#include "nucleus/logger/logger.h"

#define LOAD_FUNCTION(function) { \
    function = reinterpret_cast<PFN_##function>(GetProcAddress(hmodule, #function)); \
    if (!function) { \
        logger.error(LOG_GRAPHICS, "Could not load Vulkan extension '%s'", #function); \
        return false; \
    } \
}

namespace gfx {
namespace vulkan {

#define DECLARE_FUNCTION(function) PFN_##function function;
#define FUNCTION DECLARE_FUNCTION
#include "vulkan.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION

bool initializeVulkan() {
    HMODULE hmodule = LoadLibrary("vulkan-1.dll");
    if (!hmodule) {
        logger.error(LOG_GRAPHICS, "initializeDirect3D12: Could not load vulkan-1.dll module");
        return false;
    }

#define FUNCTION LOAD_FUNCTION
#include "vulkan.inl"
#undef FUNCTION
    return true;
}

}  // namespace vulkan
}  // namespace gfx
