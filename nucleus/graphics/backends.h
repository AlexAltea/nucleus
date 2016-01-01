/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Backends
#include "nucleus/graphics/backend/opengl/opengl_backend.h"
//#include "nucleus/graphics/backend/direct3d11/direct3d11_backend.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_backend.h"

namespace gfx {

using Direct3D12Backend = direct3d12::Direct3D12Backend;
using OpenGLBackend = opengl::OpenGLBackend;

}  // namespace gfx
