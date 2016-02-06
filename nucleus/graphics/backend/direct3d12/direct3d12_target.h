/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/target.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12ColorTarget : public ColorTarget {
public:
    ID3D12DescriptorHeap* rtvHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
};

class Direct3D12DepthStencilTarget : public DepthStencilTarget {
public:
    ID3D12DescriptorHeap* dsvHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
};

}  // namespace direct3d12
}  // namespace gfx
