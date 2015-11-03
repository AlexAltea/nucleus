/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12Backend : public IBackend {
    ID3D12Device* device;

public:
    Direct3D12Backend();
    ~Direct3D12Backend();

    virtual bool initialize(DisplayHandler display) override;

    virtual ICommandQueue* createCommandQueue() override;
    virtual ICommandBuffer* createCommandBuffer() override;
    virtual void createPipeline() override;
    virtual void createShader() override;
};

}  // namespace gfx
