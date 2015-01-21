/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Nucleus UI Language
#include "nucleus/ui/language.h"

// Nucleus UI Window object
#ifdef NUCLEUS_PLATFORM_WINDOWS
#include "nucleus/ui/windows/window.h"
#endif
#ifdef NUCLEUS_PLATFORM_LINUX
#include "nucleus/ui/linux/window.h"
#endif

#include <thread>

class UI
{
    Window* m_window;
    std::thread* m_window_thread;

public:
    Language language;

    // Initialize the UI manager, and run task() in a separate thread
    void init();

    // Creates Nucleus main window and keeps it refreshed (platform independent)
    void task();

    // Get the Nucleus window object
    Window* get();
};

extern UI ui;
