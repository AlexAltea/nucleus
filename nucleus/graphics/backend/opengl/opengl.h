/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#endif

// Declare extensions
#define DECLARE_EXTENSION(type, function) extern type function;
#define EXTENSION DECLARE_EXTENSION
#include "opengl.inc"
#undef EXTENSION
#undef DECLARE_EXTENSION

namespace graphics {

// Load extensions
bool initializeOpenGL();

}  // namespace graphics
