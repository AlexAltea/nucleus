/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define GRAPHICS_OPENGL_GL45
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#elif defined(NUCLEUS_PLATFORM_LINUX)
#define GRAPHICS_OPENGL_GL45
#include <GL/gl.h>
#include <GL/glext.h>
#include <gl/glxext.h>

#elif defined(NUCLEUS_PLATFORM_OSX)
#define GRAPHICS_OPENGL_GL41
#include <GL/gl.h>
#include <GL/glext.h>
#include <gl/glxext.h>

#elif defined(NUCLEUS_PLATFORM_ANDROID)
#define GRAPHICS_OPENGL_GLES31
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>

#elif defined(NUCLEUS_PLATFORM_IOS)
#define GRAPHICS_OPENGL_GLES31
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>
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
