/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/graphics/backend/direct3d12/direct3d12_resource.h"

namespace gfx {
namespace direct3d12 {

void* Direct3D12Resource::map() {
    D3D12_RANGE readRange = {};
    readRange.Begin = 0;
    readRange.End = 0;

    void* address = nullptr;
    resource->Map(0, &readRange, &address);
    return address;
}

bool Direct3D12Resource::unmap() {
    resource->Unmap(0, nullptr);
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
