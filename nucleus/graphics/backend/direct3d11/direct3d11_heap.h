/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/heap.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"

namespace gfx {
namespace direct3d11 {

class Direct3D11Heap : public Heap {
public:
    ID3D11DescriptorHeap* heap;
};

}  // namespace direct3d11
}  // namespace gfx
