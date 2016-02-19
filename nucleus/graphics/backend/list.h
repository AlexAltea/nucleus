/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Backends
#ifdef NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D11
#include "nucleus/graphics/backend/direct3d11/direct3d11_backend.h"
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D12
#include "nucleus/graphics/backend/direct3d12/direct3d12_backend.h"
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_OPENGL
#include "nucleus/graphics/backend/opengl/opengl_backend.h"
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_VULKAN
#include "nucleus/graphics/backend/vulkan/vulkan_backend.h"
#endif

namespace gfx {

// Shorthands
#ifdef NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D11
using Direct3D11Backend = direct3d12::Direct3D11Backend;
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D12
using Direct3D12Backend = direct3d12::Direct3D12Backend;
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_OPENGL
using OpenGLBackend = opengl::OpenGLBackend;
#endif
#ifdef NUCLEUS_FEATURE_GFXBACKEND_VULKAN
using VulkanBackend = vulkan::VulkanBackend;
#endif

}  // namespace gfx
