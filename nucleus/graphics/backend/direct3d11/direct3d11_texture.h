/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/texture.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"

namespace gfx {

class Direct3D11Texture : public ITexture {
public:
    ID3D11Texture2D* texture;
};

}  // namespace gfx
