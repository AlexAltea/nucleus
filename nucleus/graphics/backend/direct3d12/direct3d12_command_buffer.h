/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/command_buffer.h"

namespace graphics {

class Direct3D12CommandBuffer : public ICommandBuffer {
public:
    Direct3D12CommandBuffer();
    ~Direct3D12CommandBuffer();

    virtual bool reset() override;
};

}  // namespace graphics
