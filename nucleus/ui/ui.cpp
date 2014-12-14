/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"

#ifdef NUCLEUS_WIN
#include "windows/window.h"
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
    Window mainWindow("Nucleus", 960, 544);
    mainWindow.init();
    mainWindow.loop();
#endif
}
