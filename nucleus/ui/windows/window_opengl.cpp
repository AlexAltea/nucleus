/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window_opengl.h"

#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")

void OpenGL_Init(HWND hWnd)
{
    HDC hDC;
    HGLRC hRC;
    GLuint PixelFormat;

    static const PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // Size of this Pixel Format Descriptor
        1,                              // Version
        PFD_DRAW_TO_WINDOW |            // Format must support Window
        PFD_SUPPORT_OPENGL |            // Format must support OpenGL
        PFD_DOUBLEBUFFER,               // Format must support double buffering
        PFD_TYPE_RGBA,                  // Request an RGBA format
        24,                             // Select our color depth
        0, 0, 0, 0, 0, 0,               // Color bits ignored
        8,                              // No alpha buffer
        0,                              // Shift bit ignored
        0,                              // No accumulation Buffer
        0, 0, 0, 0,                     // Accumulation bits Ignored
        16,                             // At least a 16-bit Z-buffer (depth buffer)  
        8,                              // 8-bit stencil buffer
        0,                              // No auxiliary buffer
        PFD_MAIN_PLANE,                 // Main drawing layer
        0,                              // Reserved
        0, 0, 0                         // Layer masks ignored
    };

    hDC = GetDC(hWnd);
    if (!hDC) {
		MessageBox(hWnd, "Failed to get a device context.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    PixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (!PixelFormat) {
		MessageBox(hWnd, "Can't find a suitable PixelFormat.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    if (!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		MessageBox(hWnd, "Can't set the PixelFormat.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    hRC = wglCreateContext(hDC);
    if (!hRC) {
		MessageBox(hWnd, "Can't create a GL rendering context.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    if (!wglMakeCurrent(hDC, hRC)) {
		MessageBox(hWnd, "Can't activate the GL rendering context.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    /*if (glewInit() != GLEW_OK) {
		MessageBox(hWnd, "Failed to re-initialize GLEW.", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
		return;
	}*/
}
