/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

#include <vector>

namespace gfx {

// Command type
enum OpenGLCommandType {
    OPENGL_CMD_BIND_PIPELINE,
    OPENGL_CMD_CLEAR_COLOR,
    OPENGL_CMD_CLEAR_DEPTH_STENCIL,
    OPENGL_CMD_DRAW,
    OPENGL_CMD_DRAW_INDEXED,
    OPENGL_CMD_DRAW_INDIRECT,
    OPENGL_CMD_DRAW_INDEXED_INDIRECT,
};

// Command arguments
union OpenGLCommandData {
    struct ClearColor {
        GLuint framebuffer;
        GLint drawbuffer;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;
    } clearColor;

    struct ClearDepthStencil {
        GLuint framebuffer;
        GLint drawbuffer;
        GLfloat depth;
        GLint stencil;
    } clearDepthStencil;
};

struct OpenGLCommand {
    OpenGLCommandType type;
    OpenGLCommandData data;
};

class OpenGLCommandBuffer : public ICommandBuffer {
public:
    // Holds the commands to be pushed
    std::vector<OpenGLCommand> commands;

    OpenGLCommandBuffer();
    ~OpenGLCommandBuffer();

    virtual bool reset() override;

    // Commands
    virtual void cmdBindPipeline(IPipelineState* pipeline) override;
    virtual void cmdClearColor(IColorTarget* target, const F32* colorValue) override;
    virtual void cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) override;
    virtual void cmdDraw() override;
    virtual void cmdDrawIndexed() override;
    virtual void cmdDrawIndirect() override;
    virtual void cmdDrawIndexedIndirect() override;
};

}  // namespace gfx
