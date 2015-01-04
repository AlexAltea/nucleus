/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_renderer.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_enum.h"
#include "nucleus/gpu/rsx_methods.h"

#include "nucleus/gpu/opengl/opengl_vp.h"
#include "nucleus/gpu/opengl/opengl_fp.h"

// OpenGL dependencies
#include <GL/glew.h>

#define checkRendererError(name) \
    if (glGetError() != GL_NO_ERROR) { \
        nucleus.log.error(LOG_GPU, "Something went wrong in %s", name); \
    }

PGRAPH_OpenGL::PGRAPH_OpenGL()
{
    // Wait until Nucleus window is ready
    while (!ui.get()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    m_window = ui.get();
    m_window->init();
}

void PGRAPH_OpenGL::AlphaFunc(u32 func, f32 ref)
{
    glAlphaFunc(func, ref);
    checkRendererError("AlphaFunc");
}

// NOTE: RSX doesn't know how big the vertex buffer is, but OpenGL requires this information
// to copy the data to the host GPU. Therefore, LoadVertexAttributes needs to be called.
void PGRAPH_OpenGL::BindVertexAttributes()
{
    // Indices are attr.type
    static const GLenum vertexType[] = {
        0, GL_SHORT, GL_FLOAT, GL_HALF_FLOAT, GL_UNSIGNED_BYTE, GL_SHORT, GL_FLOAT, GL_UNSIGNED_BYTE
    };
    static const GLboolean vertexNormalized[] = {
        0, GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (int index = 0; index < 16; index++) {
        const auto& attr = vpe.attr[index];
        if (attr.size) {
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, attr.data.size(), attr.data.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, attr.size, vertexType[attr.type], vertexNormalized[attr.type], 0, 0);
        }
    }
}

void PGRAPH_OpenGL::Begin(u32 mode)
{
    checkRendererError("Begin");
}

void PGRAPH_OpenGL::ClearColor(u8 a, u8 r, u8 g, u8 b)
{
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    checkRendererError("ClearColor");
}

void PGRAPH_OpenGL::ClearDepth(u32 value)
{
    glClearDepth(value / (float)0xFFFFFF);
    checkRendererError("ClearDepth");
}

void PGRAPH_OpenGL::ClearStencil(u32 value)
{
    glClearStencil(value);
    checkRendererError("ClearStencil");
}

void PGRAPH_OpenGL::ClearSurface(u32 mask)
{
    // Set mask
    GLbitfield clearMask = 0;
    if (mask & 0x01) clearMask |= GL_DEPTH_BUFFER_BIT;
    if (mask & 0x02) clearMask |= GL_STENCIL_BUFFER_BIT;
    if (mask & 0xF0) clearMask |= GL_COLOR_BUFFER_BIT;

    glClear(clearMask);
    checkRendererError("ClearSurface");
}

void PGRAPH_OpenGL::ColorMask(bool a, bool r, bool g, bool b)
{
    glColorMask(r, g, b, a);
    checkRendererError("ColorMask");
}

void PGRAPH_OpenGL::DepthFunc(u32 func)
{
    glDepthFunc(func);
    checkRendererError("DepthFunc");
}

void PGRAPH_OpenGL::DrawArrays(u32 mode, u32 first, u32 count)
{
    u64 hash_vp = 0;
    if (cache_vp.find(hash_vp) == cache_vp.end()) {
        OpenGLVertexProgram vp;
        vp.decompile(vpe.data, vpe.start);
        vp.compile();
        cache_vp[hash_vp] = vp;
    }

    u64 hash_fp = 0;
    if (cache_fp.find(hash_fp) == cache_fp.end()) {
        OpenGLFragmentProgram fp;
        fp.decompile(nucleus.memory.ptr<rsx_fp_instruction_t>((fp_location ? nucleus.rsx.io_address : 0xC0000000) + fp_offset));
        fp.compile();
        cache_fp[hash_fp] = fp;
    }

    // Link, validate and use program
    GLuint id = glCreateProgram();
    glAttachShader(id, cache_vp[hash_vp].id);
    glAttachShader(id, cache_fp[hash_fp].id);
    glLinkProgram(id);
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        nucleus.log.error(LOG_GPU, "PGRAPH_OpenGL::DrawArrays: Can't link program");
    }
    glGetProgramiv(id, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        nucleus.log.error(LOG_GPU, "PGRAPH_OpenGL::DrawArrays: Can't validate program");
    }
    glUseProgram(id);

    // Upload VP constants
    for (u32 i = 0; i < 468; i++) {
        auto& constant = vpe.constant[i];
        if (constant.dirty) {
            GLint loc = glGetUniformLocation(id, format("c[%d]", i).c_str());
            glUniform4f(loc, constant.x, constant.y, constant.z, constant.w);
            constant.dirty = false;
        }
    }

    glDrawArrays(GL_TRIANGLES, first, count);
    checkRendererError("DrawArrays");
}

void PGRAPH_OpenGL::Enable(u32 prop, u32 enabled)
{
    switch (prop) {
    case NV4097_SET_DITHER_ENABLE:
        enabled ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
        break;

    case NV4097_SET_ALPHA_TEST_ENABLE:
        enabled ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
        break;

    case NV4097_SET_STENCIL_TEST_ENABLE:
        enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        break;

    case NV4097_SET_DEPTH_TEST_ENABLE:
        enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        break;

    case NV4097_SET_CULL_FACE_ENABLE:
        enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        break;

    case NV4097_SET_BLEND_ENABLE:
        enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        break;

    case NV4097_SET_POLY_OFFSET_FILL_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
        break;

    case NV4097_SET_POLY_OFFSET_LINE_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
        break;

    case NV4097_SET_POLY_OFFSET_POINT_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
        break;

    case NV4097_SET_LOGIC_OP_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LOGIC_OP) : glDisable(GL_LOGIC_OP);
        break;

    case NV4097_SET_SPECULAR_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
        break;

    case NV4097_SET_LINE_SMOOTH_ENABLE:
        enabled ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
        break;

    case NV4097_SET_POLY_SMOOTH_ENABLE:
        enabled ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
        break;
    }
    checkRendererError("Enable");
}

void PGRAPH_OpenGL::End()
{
    checkRendererError("End");
}

void PGRAPH_OpenGL::Flip()
{
    m_window->swap_buffers();
}

void PGRAPH_OpenGL::UnbindVertexAttributes()
{
    for (int index = 0; index < 16; index++) {
        glDisableVertexAttribArray(index);
        checkRendererError("UnbindVertexAttributes");
    }
    glBindVertexArray(0);
}
