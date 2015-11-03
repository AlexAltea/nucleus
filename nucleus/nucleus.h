/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

void nucleusConfigure(int argc, char **argv);

// Initialize UI
#if defined(NUCLEUS_PLATFORM_WINDOWS)
void nucleusPrepare(HWND hwnd, HDC hdc, int width, int height);
#elif defined(NUCLEUS_PLATFORM_LINUX)
void nucleusPrepare(Display* display);
#endif

int nucleusInitialize(int argc, char **argv);

// Events
void nucleusOnResize(unsigned int w, unsigned int h, unsigned int dpi, unsigned int hz);
