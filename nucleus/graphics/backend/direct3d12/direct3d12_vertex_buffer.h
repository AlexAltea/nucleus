/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_resource.h"
#include "nucleus/graphics/vertex_buffer.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12VertexBuffer : public Direct3D12Resource, public VertexBuffer {
public:
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

    Direct3D12VertexBuffer(ID3D12Device* device, UINT64 size);
    ~Direct3D12VertexBuffer();
};

}  // namespace direct3d12
}  // namespace gfx
