/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/renderer.h"
#include "nucleus/ui/ui.h"

// OpenGL dependencies
#ifdef NUCLEUS_WIN
#include <Windows.h>
#include <GL/gl.h>
#endif
#ifdef NUCLEUS_LINUX
#include <GL/gl.h>
#endif

class RSXRendererOpenGL : public RSXRenderer {
    Window* m_window;

public:
    RSXRendererOpenGL();

    virtual void AlphaFunc(u32 func, f32 ref) override;
    virtual void Begin(u32 mode) override;
    virtual void DrawArrays(u32 mode, u32 first, u32 count) override;
    virtual void Enable(u32 prop, u32 enabled) override;
    virtual void End() override;
    virtual void Flip() override;
};
