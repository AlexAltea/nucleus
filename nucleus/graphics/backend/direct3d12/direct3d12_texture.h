/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/texture.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12Texture : public ITexture {
public:
    ID3D12Resource* resource;
};

}  // namespace gfx
