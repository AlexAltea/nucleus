/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_renderer.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_enum.h"
#include "nucleus/gpu/rsx_methods.h"

// OpenGL dependencies
#include <GL/glew.h>

#define checkRendererError(name) \
    if (glGetError() != GL_NO_ERROR) { \
        nucleus.log.error(LOG_GPU, "Something went wrong"); \
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

void PGRAPH_OpenGL::BindVertexAttributes()
{
    for (int index = 0; index < 16; index++) {
        const auto& attr = vp_attr[index];
        if (attr.dirty) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, attr.size, GL_FLOAT, GL_FALSE, attr.stride, 0);
        }
    }
}

void PGRAPH_OpenGL::Begin(u32 mode)
{
    glBegin(mode);
    checkRendererError("Begin");
}

void PGRAPH_OpenGL::DrawArrays(u32 mode, u32 first, u32 count)
{
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
        enabled ? glEnable(GL_LOGIC_OP) : glDisable(GL_LOGIC_OP);
        break;

    case NV4097_SET_SPECULAR_ENABLE:
        enabled ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
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
    glEnd();
    checkRendererError("End");
}

void PGRAPH_OpenGL::Flip()
{
    m_window->swap_buffers();
}
