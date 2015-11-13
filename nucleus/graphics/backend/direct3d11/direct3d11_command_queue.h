/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"

namespace gfx {

class Direct3D11CommandQueue : public ICommandQueue {
private:
    ID3D11DeviceContext* immediateContext;

public:
    Direct3D11CommandQueue();
    ~Direct3D11CommandQueue();

    bool initialize(ID3D11Device* device);

    void submit(ICommandBuffer* cmdBuffer) override;
    void waitIdle() override;
};

}  // namespace gfx
