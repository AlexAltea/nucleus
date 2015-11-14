/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/target.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12ColorTarget : public ColorTarget {
public:
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
};

class Direct3D12DepthStencilTarget : public DepthStencilTarget {
public:
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
};

}  // namespace gfx
