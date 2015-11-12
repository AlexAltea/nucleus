/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/target.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"

namespace gfx {

class Direct3D11ColorTarget : public IColorTarget {
public:
    D3D11_CPU_DESCRIPTOR_HANDLE handle;
};

class Direct3D11DepthStencilTarget : public IDepthStencilTarget {
public:
    D3D11_CPU_DESCRIPTOR_HANDLE handle;
};

}  // namespace gfx
