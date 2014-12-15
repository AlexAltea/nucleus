/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/renderer.h"

// OpenGL dependencies
#ifdef NUCLEUS_WIN
#include <Windows.h>
#include <GL/gl.h>
#endif
#ifdef NUCLEUS_LINUX
#include <GL/gl.h>
#endif

class RSXRendererOpenGL : public RSXRenderer {

public:
    virtual void Begin(u32 mode) override;
    virtual void DrawArrays(u32 count, const be_t<u32>* args) override;
    virtual void End() override;
};
