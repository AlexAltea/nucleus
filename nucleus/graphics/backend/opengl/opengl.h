/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#if defined(NUCLEUS_PLATFORM_ANDROID)
#define GRAPHICS_OPENGL_API_EGL
#define GRAPHICS_OPENGL_GLES
#define GRAPHICS_OPENGL_GLES31
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>

#elif defined(NUCLEUS_PLATFORM_IOS)
#define GRAPHICS_OPENGL_API_EGL
#define GRAPHICS_OPENGL_GLES
#define GRAPHICS_OPENGL_GLES31
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>

#elif defined(NUCLEUS_PLATFORM_LINUX)
#define GRAPHICS_OPENGL_API_GLX
#define GRAPHICS_OPENGL_GL
#define GRAPHICS_OPENGL_GL45
#include <GL/gl.h>
#include <GL/glext.h>
#include <gl/glxext.h>

#elif defined(NUCLEUS_PLATFORM_OSX)
#define GRAPHICS_OPENGL_API_GLX
#define GRAPHICS_OPENGL_GL
#define GRAPHICS_OPENGL_GL41
#include <GL/gl.h>
#include <GL/glext.h>
#include <gl/glxext.h>

#elif defined(NUCLEUS_PLATFORM_UWP)
#define GRAPHICS_OPENGL_API_EGL
#define GRAPHICS_OPENGL_GLES
#define GRAPHICS_OPENGL_GLES30
#include <EGL/egl.h>
#include <GLES3/gl31.h>

#elif defined(NUCLEUS_PLATFORM_WINDOWS)
#define GRAPHICS_OPENGL_API_WGL
#define GRAPHICS_OPENGL_GL
#define GRAPHICS_OPENGL_GL45
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#endif

namespace gfx {
namespace opengl {

// Declare extensions
#define DECLARE_EXTENSION(type, function) extern type function;
#define EXTENSION DECLARE_EXTENSION
#include "opengl.inl"
#undef EXTENSION
#undef DECLARE_EXTENSION

// Load extensions
bool initializeOpenGL();

}  // namespace opengl
}  // namespace gfx
