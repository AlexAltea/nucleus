/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_renderer.h"
#include "nucleus/gpu/rsx_enum.h"

#pragma comment(lib, "opengl32.lib")

void RSXRendererOpenGL::Begin(u32 mode)
{
    glBegin(mode);
}

void RSXRendererOpenGL::DrawArrays(u32 count,const be_t<u32>* args)
{
}

void RSXRendererOpenGL::End()
{
    glEnd();
}
