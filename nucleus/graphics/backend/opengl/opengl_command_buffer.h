/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl.h"
#include "nucleus/graphics/backend/opengl/opengl_fence.h"
#include "nucleus/graphics/backend/opengl/opengl_target.h"
#include "nucleus/graphics/backend/opengl/opengl_texture.h"
#include "nucleus/graphics/backend/opengl/opengl_vertex_buffer.h"

#include <vector>

namespace gfx {

// Command
struct OpenGLCommand {
    enum Type {
        // Public
        TYPE_BIND_PIPELINE,
        TYPE_CLEAR_COLOR,
        TYPE_CLEAR_DEPTH_STENCIL,
        TYPE_DRAW,
        TYPE_DRAW_INDEXED,
        TYPE_SET_TARGETS,
        TYPE_SET_VIEWPORTS,
        TYPE_SET_SCISSORS,

        // Private
        TYPE_INTERNAL_CREATE_TEXTURE,
        TYPE_INTERNAL_CREATE_VERTEXBUFFER,
        TYPE_INTERNAL_SIGNAL_FENCE,
    } type;

    // Constructor
    OpenGLCommand(Type type) : type(type) {}
};

// Public commands
struct OpenGLCommandClearColor : public OpenGLCommand {
    OpenGLCommandClearColor() : OpenGLCommand(TYPE_CLEAR_COLOR) {}

    OpenGLColorTarget* target;
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

struct OpenGLCommandDraw : public OpenGLCommand {
    OpenGLCommandDraw() : OpenGLCommand(TYPE_DRAW) {}

    GLint firstVertex;
    GLsizei vertexCount;
    GLuint firstInstance;
    GLsizei instanceCount;
};

struct OpenGLCommandDrawIndexed : public OpenGLCommand {
    OpenGLCommandDrawIndexed() : OpenGLCommand(TYPE_DRAW_INDEXED) {}

    GLint firstIndex;
    GLsizei indexCount;
    GLint vertexOffset;
    GLuint firstInstance;
    GLsizei instanceCount;
};

struct OpenGLCommandSetTargets : public OpenGLCommand {
    OpenGLCommandSetTargets() : OpenGLCommand(TYPE_SET_TARGETS) {}

    U32 colorCount;
    GLuint colorTargets[32];
    GLuint depthStencilTarget;
};

struct OpenGLCommandSetViewports : public OpenGLCommand {
    OpenGLCommandSetViewports() : OpenGLCommand(TYPE_SET_VIEWPORTS) {}

    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
};

struct OpenGLCommandSetScissors : public OpenGLCommand {
    OpenGLCommandSetScissors() : OpenGLCommand(TYPE_SET_VIEWPORTS) {}

    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
};

// Private commands
struct OpenGLCommandInternalCreateTexture : public OpenGLCommand {
    OpenGLCommandInternalCreateTexture() : OpenGLCommand(TYPE_INTERNAL_CREATE_TEXTURE) {}
    OpenGLTexture* texture;
};
struct OpenGLCommandInternalCreateVertexBuffer : public OpenGLCommand {
    OpenGLCommandInternalCreateVertexBuffer() : OpenGLCommand(TYPE_INTERNAL_CREATE_VERTEXBUFFER) {}
    OpenGLVertexBuffer* vtxBuffer;
};
struct OpenGLCommandInternalSignalFence : public OpenGLCommand {
    OpenGLCommandInternalSignalFence() : OpenGLCommand(TYPE_INTERNAL_SIGNAL_FENCE) {}
    OpenGLFence* fence;
};

// Command buffer
class OpenGLCommandBuffer : public CommandBuffer {
public:
    // Holds the commands to be pushed
    std::vector<OpenGLCommand*> commands;

    OpenGLCommandBuffer();
    ~OpenGLCommandBuffer();

    virtual bool reset() override;

    // Commands
    virtual void cmdBindPipeline(IPipelineState* pipeline) override;
    virtual void cmdClearColor(ColorTarget* target, const F32* colorValue) override;
    virtual void cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) override;
    virtual void cmdDraw(U32 firstVertex, U32 vertexCount, U32 firstInstance, U32 instanceCount) override;
    virtual void cmdDrawIndexed(U32 firstIndex, U32 indexCount, U32 vertexOffset, U32 firstInstance, U32 instanceCount) override;
    virtual void cmdSetVertexBuffers() override;
    virtual void cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) override;
    virtual void cmdSetPrimitiveTopology(PrimitiveTopology topology) override;
    virtual void cmdSetViewports(U32 viewportsCount, const Viewport* viewports) override;
    virtual void cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) override;
};

}  // namespace gfx
