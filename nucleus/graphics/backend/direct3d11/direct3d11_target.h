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
    ID3D11RenderTargetView* view;
};

class Direct3D11DepthStencilTarget : public IDepthStencilTarget {
public:
    ID3D11DepthStencilView* view;
};

}  // namespace gfx
