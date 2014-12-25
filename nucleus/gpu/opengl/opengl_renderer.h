/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_pgraph.h"
#include "nucleus/ui/ui.h"

class PGRAPH_OpenGL : public PGRAPH {
    Window* m_window;

public:
    PGRAPH_OpenGL();

    virtual void AlphaFunc(u32 func, f32 ref) override;
    virtual void Begin(u32 mode) override;
    virtual void BindVertexAttributes() override;
    virtual void DrawArrays(u32 mode, u32 first, u32 count) override;
    virtual void Enable(u32 prop, u32 enabled) override;
    virtual void End() override;
    virtual void Flip() override;
    virtual void UnbindVertexAttributes() override;
};
