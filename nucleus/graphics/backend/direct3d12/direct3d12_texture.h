/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/texture.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12Texture : public Texture {
public:
    ID3D12Resource* resource;

    Direct3D12Texture(ID3D12Device* device, const TextureDesc& desc);
    ~Direct3D12Texture();

    /**
     * Upload texture data
     * @param[in]  device  Device that will receive the texture data
     * @param[in]  queue   Command queue to that will transfer the data
     * @param[in]  data    Pointer to the texture data
     * @param[in]  size    Size of the texture data
     */
    bool upload(ID3D12Device* device, ID3D12CommandQueue* queue, const Byte* data, Size size);
};

}  // namespace direct3d12
}  // namespace gfx
