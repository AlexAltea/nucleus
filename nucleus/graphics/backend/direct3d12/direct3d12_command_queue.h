/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/command_queue.h"

namespace gfx {

class Direct3D12CommandQueue : public ICommandQueue {
public:
    Direct3D12CommandQueue();
    ~Direct3D12CommandQueue();

    void submit(ICommandBuffer* cmdBuffer) override;
};

}  // namespace gfx
