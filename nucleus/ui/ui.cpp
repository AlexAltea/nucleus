/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/config.h"

#ifdef NUCLEUS_WIN
#include "windows/window_opengl.h"
#endif
#ifdef NUCLEUS_LINUX
#include "linux/window_opengl.h"
#endif

// Global UI manager object
UI ui;

void UI::init()
{
    m_window_thread = new std::thread([&](){
        task();
    });
}

void UI::task()
{
#ifdef NUCLEUS_WIN
    switch (config.gpuBackend) {
    case GPU_BACKEND_OPENGL:
        m_window = new WindowOpenGL("Nucleus", 960, 544);
        m_window->loop();
        break;

    case GPU_BACKEND_NULL:
    case GPU_BACKEND_SOFTWARE:
    case GPU_BACKEND_DIRECT3D:
    default:
        break;
    }
#endif
}

Window* UI::get()
{
    return m_window;
}
