/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_debug.h"

namespace gfx {
namespace vulkan {

void VulkanDebug::enable() {
    validationLayers.clear();
    validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
}

}  // namespace vulkan
}  // namespace gfx
