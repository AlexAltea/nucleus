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

#ifdef NUCLEUS_PLATFORM_LINUX
#endif

#ifdef NUCLEUS_PLATFORM_OSX
#endif

#ifdef NUCLEUS_PLATFORM_ANDROID
#endif

#ifdef NUCLEUS_PLATFORM_IOS
#endif

// Declare extensions
#define DECLARE_EXTENSION(type, function) extern type function;
#define EXTENSION DECLARE_EXTENSION
#include "opengl_tables.h"
#undef EXTENSION
#undef DECLARE_EXTENSION

// Load extensions
bool openglInit();
