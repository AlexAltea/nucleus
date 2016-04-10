/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Platform-specific definitions
#if defined(NUCLEUS_TARGET_ANDROID)
#define VK_USE_TARGET_ANDROID_KHR
#elif defined(NUCLEUS_TARGET_LINUX)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(NUCLEUS_TARGET_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Vulkan
#include "externals/vulkan/vulkan.h"
#include "externals/vulkan/vk_platform.h"

// SPIR-V
#include "externals/spirv/GLSL450Lib.h"
#include "externals/spirv/spirv.hpp"

namespace gfx {
namespace vulkan {

// Load extensions
bool initializeVulkan();

}  // namespace vulkan
}  // namespace gfx
