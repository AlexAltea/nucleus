/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

#include <vector>

namespace gfx {

// Command
struct OpenGLCommand {
    enum Type {
        TYPE_BIND_PIPELINE,
        TYPE_CLEAR_COLOR,
        TYPE_CLEAR_DEPTH_STENCIL,
        TYPE_DRAW,
        TYPE_DRAW_INDEXED,
        TYPE_DRAW_INDIRECT,
        TYPE_DRAW_INDEXED_INDIRECT,
    } type;

    // Constructor
    OpenGLCommand(Type type) : type(type) {}
};

struct OpenGLCommandClearColor : public OpenGLCommand {
    OpenGLCommandClearColor() : OpenGLCommand(TYPE_CLEAR_COLOR) {}

    GLuint framebuffer;
    GLint drawbuffer;
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

struct OpenGLCommandClearDepthStencil : public OpenGLCommand {
    OpenGLCommandClearDepthStencil() : OpenGLCommand(TYPE_CLEAR_DEPTH_STENCIL) {}

    GLuint framebuffer;
    GLint drawbuffer;
    GLfloat depth;
    GLint stencil;
};

// Command buffer
class OpenGLCommandBuffer : public ICommandBuffer {
public:
    // Holds the commands to be pushed
    std::vector<OpenGLCommand*> commands;

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
