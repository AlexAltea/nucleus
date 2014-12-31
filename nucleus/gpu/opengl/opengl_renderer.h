/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/rsx_pgraph.h"
#include "nucleus/gpu/opengl/opengl_vp.h"
#include "nucleus/gpu/opengl/opengl_fp.h"
#include "nucleus/ui/ui.h"

#include <unordered_map>

class PGRAPH_OpenGL : public PGRAPH {
    Window* m_window;

    // Cache
    std::unordered_map<u64, OpenGLVertexProgram> cache_vp;
    std::unordered_map<u64, OpenGLFragmentProgram> cache_fp;

public:
    PGRAPH_OpenGL();

    virtual void AlphaFunc(u32 func, f32 ref) override;
    virtual void Begin(u32 mode) override;
    virtual void BindVertexAttributes() override;
    virtual void ClearColor(u8 a, u8 r, u8 g, u8 b) override;
    virtual void ClearDepth(u32 value) override;
    virtual void ClearStencil(u32 value) override;
    virtual void ClearSurface(u32 mask) override;
    virtual void ColorMask(bool a, bool r, bool g, bool b) override;
    virtual void DepthFunc(u32 func) override;
    virtual void DrawArrays(u32 mode, u32 first, u32 count) override;
    virtual void Enable(u32 prop, u32 enabled) override;
    virtual void End() override;
    virtual void Flip() override;
    virtual void UnbindVertexAttributes() override;
};
