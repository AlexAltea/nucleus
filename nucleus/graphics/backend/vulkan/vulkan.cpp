/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan.h"

#define LOAD_EXTENSION(function) { \
    function = reinterpret_cast<PFN_##function>(handle, vkGetInstanceProcAddr(#function)); \
    if (!function) { \
        logger.error(LOG_GRAPHICS, "Could not load Vulkan extension '%s'", #function); \
        return false; \
    } \
}

namespace gfx {
namespace vulkan {

bool initializeVulkan() {
    return true;
}

}  // namespace vulkan
}  // namespace gfx
