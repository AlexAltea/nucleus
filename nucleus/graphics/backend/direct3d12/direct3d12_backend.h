/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

namespace gfx {

class Direct3D12Backend : public IBackend {
public:
    Direct3D12Backend();
    ~Direct3D12Backend();

    virtual ICommandQueue* createCommandQueue() override;
    virtual ICommandBuffer* createCommandBuffer() override;
    virtual void createPipeline() override;
    virtual void createShader() override;
};

}  // namespace gfx
