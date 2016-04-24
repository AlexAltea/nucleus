/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#if defined(NUCLEUS_TARGET_LINUX)
#include <X11/Xlib.h>
#elif defined(NUCLEUS_TARGET_UWP)
#include <Windows.h>
#elif defined(NUCLEUS_TARGET_WINDOWS)
#include <Windows.h>
#endif

void nucleusConfigure(int argc, char **argv);

// Initialize UI
#if defined(NUCLEUS_TARGET_UWP)
void nucleusPrepare(IUnknown* window, int width, int height);
#elif defined(NUCLEUS_TARGET_WINDOWS)
void nucleusPrepare(HWND hwnd, HDC hdc, int width, int height);
#elif defined(NUCLEUS_TARGET_LINUX)
void nucleusPrepare(Display* display);
#endif

int nucleusInitialize(int argc, char **argv);
void nucleusFinalize();

// Events
void nucleusOnWindowSizeChanged(unsigned int width, unsigned int height);
void nucleusOnMouseClick(int x, int y);
void nucleusOnMouseDoubleClick(int x, int y);
void nucleusOnMouseMove(int x, int y);
void nucleusOnMouseWheel(int x, int y, int delta);
void nucleusOnKeyDown(int keycode);
void nucleusOnKeyUp(int keycode);

// Drag-and-Drop events
void nucleusOnDragEnter();
void nucleusOnDragOver();
void nucleusOnDragLeave();
void nucleusOnDrop();
