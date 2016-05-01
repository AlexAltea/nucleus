/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <vector>

namespace gfx {
namespace vulkan {

class VulkanDebug {
public:
    std::vector<const char*> validationLayers;

    /**
     * Specify debugging layers
     */
    void enable();
};

}  // namespace vulkan
}  // namespace gfx
