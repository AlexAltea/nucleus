/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/language.h"
#include "nucleus/ui/screen.h"

#include <queue>
#include <thread>
#include <vector>

class UI
{
    std::thread* m_thread;
    std::vector<Screen*> m_screens;

    // New screens to be added
    std::queue<Screen*> m_new_screens;

    // Connect to the global UI
    void connect();

    // Swaps the buffers updating the contents of the window (platform independent)
    void swap_buffers();

public:
    Language language;

    // Surface properties
    bool surfaceChanged = false;
    unsigned int surfaceWidth = 0;
    unsigned int surfaceHeight = 0;
    unsigned int surfaceDpi = 100;
    unsigned int surfaceHz = 60;
    float surfaceProportion = 1.0;

    // RSX connection
    bool rsxChanged = false;

    // Initialize the UI manager, and run task() in a separate thread
    void init();

    void task();

    void resize();

    void push_screen(Screen* screen);
};

extern UI ui;
