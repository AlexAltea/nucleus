/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/language.h"
#include "nucleus/ui/screen.h"

#include <queue>
#include <thread>
#include <vector>

namespace ui {

class UI {
    std::shared_ptr<gfx::IBackend> graphics;

    std::thread thread;
    std::vector<Screen*> m_screens;

    // New screens to be added
    std::queue<Screen*> m_new_screens;

public:
    Language language;

    // Surface properties
    bool surfaceChanged = false;
    unsigned int surfaceWidth = 0;
    unsigned int surfaceHeight = 0;
    unsigned int surfaceDpi = 100;
    unsigned int surfaceHz = 60;
    float surfaceProportion = 1.0;

    // Constructor
    UI(std::shared_ptr<gfx::IBackend> graphics);

    // RSX connection
    bool rsxChanged = false;

    bool initialize();

    void task();

    void resize();

    void push_screen(Screen* screen);
};

}  // namespace ui
