/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12CommandQueue : public ICommandQueue {
private:
    ID3D12CommandQueue* queue;

public:
    Direct3D12CommandQueue();
    ~Direct3D12CommandQueue();

    bool initialize(ID3D12Device* device);

    void submit(ICommandBuffer* cmdBuffer) override;
    void waitIdle() override;
};

}  // namespace gfx
