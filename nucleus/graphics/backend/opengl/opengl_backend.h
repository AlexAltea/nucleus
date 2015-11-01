/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

namespace gfx {

// Forward declarations
class IBackend;

class OpenGLBackend : public IBackend {
public:
    OpenGLBackend();
    ~OpenGLBackend();

    // Command buffer management
    virtual ICommandBuffer* createCommandBuffer() override;

    // Pipeline management
    virtual void createPipeline() override;
};

}  // namespace gfx
