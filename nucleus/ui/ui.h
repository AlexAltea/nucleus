/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <thread>

class UI
{
    std::thread* m_window_thread;

public:
    // Initialize the UI manager, and run task() in a separate thread
    void init();

    // Creates Nucleus main window and keeps it refreshed (platform independent)
    void task();
};

extern UI ui;
