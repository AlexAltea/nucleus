/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/vertex_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12VertexBuffer : public VertexBuffer {
public:
    ID3D12Resource* resource;

    Direct3D12VertexBuffer(ID3D12Device* device, UINT64 size);
    ~Direct3D12VertexBuffer();

    virtual void* map() override;
    virtual bool unmap() override;
};

}  // namespace direct3d12
}  // namespace gfx
