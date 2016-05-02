/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan.h"
#include "nucleus/logger/logger.h"

#if defined(NUCLEUS_TARGET_LINUX)
#include <dlfcn.h>
#endif

// Load functions
#if defined(NUCLEUS_TARGET_LINUX)
#define LOAD_FUNCTION(function) { \
    function = reinterpret_cast<PFN_##function>(dlsym(module, #function)); \
    if (!function) { \
        logger.error(LOG_GRAPHICS, "Could not load Vulkan extension '%s'", #function); \
        return false; \
    } \
}
#elif defined(NUCLEUS_TARGET_WINDOWS)
#define LOAD_FUNCTION(function) { \
    function = reinterpret_cast<PFN_##function>(GetProcAddress(module, #function)); \
    if (!function) { \
        logger.error(LOG_GRAPHICS, "Could not load Vulkan extension '%s'", #function); \
        return false; \
    } \
}
#endif

namespace gfx {
namespace vulkan {

#define DECLARE_FUNCTION(function) PFN_##function function;
#define FUNCTION DECLARE_FUNCTION
#include "vulkan.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION

bool initializeVulkan() {
#if defined(NUCLEUS_TARGET_LINUX)
    void* module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!module) {
        logger.error(LOG_GRAPHICS, "initializeVulkan: Could not load libvulkan.so module");
        return false;
    }
#elif defined(NUCLEUS_TARGET_WINDOWS)
    HMODULE module = LoadLibrary("vulkan-1.dll");
    if (!module) {
        logger.error(LOG_GRAPHICS, "initializeVulkan: Could not load vulkan-1.dll module");
        return false;
    }
#endif

#define FUNCTION LOAD_FUNCTION
#include "vulkan.inl"
#undef FUNCTION
    return true;
}

}  // namespace vulkan
}  // namespace gfx
