/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

namespace graphics {

class OpenGLBackend : public IBackend {
public:
    OpenGLBackend();
    ~OpenGLBackend();

    // Command queue management
    virtual void CommandQueueSubmit() override;

    // Command buffer management
    virtual void CreateCommandBuffer() override;
    virtual void BeginCommandBuffer() override;
    virtual void EndCommandBuffer() override;
    virtual void ResetCommandBuffer() override;

    // Pipeline management
    virtual void CreatePipeline() override;
};

}  // namespace graphics
