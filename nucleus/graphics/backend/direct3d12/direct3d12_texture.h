/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_resource.h"
#include "nucleus/graphics/texture.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12Texture : public Direct3D12Resource, public Texture {
public:
    ID3D12DescriptorHeap* srvHeap;

    Direct3D12Texture(ID3D12Resource* resource);
    Direct3D12Texture(ID3D12Device* device, const TextureDesc& desc);
    ~Direct3D12Texture();

    /**
     * Upload texture data
     * @param[in]  device  Device that will receive the texture data
     * @param[in]  queue   Command queue to that will transfer the data
     * @param[in]  desc    Texture description with valid data and size parameters
     */
    bool upload(ID3D12Device* device, ID3D12CommandQueue* queue, const TextureDesc& desc);
};

}  // namespace direct3d12
}  // namespace gfx
