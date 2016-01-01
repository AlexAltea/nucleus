/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
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
    // Graphics
    std::shared_ptr<gfx::IBackend> graphics;
    gfx::CommandQueue* cmdQueue;
    gfx::CommandBuffer* cmdBuffer;

    std::thread thread;
    std::vector<std::unique_ptr<Screen>> screens;
    std::queue<std::unique_ptr<Screen>> newScreens;

public:
    Language language;

    // Surface properties
    unsigned int surfaceWidth = 0;
    unsigned int surfaceHeight = 0;
    unsigned int surfaceDpi = 100;
    unsigned int surfaceHz = 60;
    float surfaceProportion = 1.0;

    // Constructor
    UI(std::shared_ptr<gfx::IBackend> graphics, gfx::CommandQueue* queue);

    // RSX connection
    bool rsxChanged = false;

    bool initialize();

    void task();

    /**
     * Pushes a new screen to the back of the screen array
     * @param[in]  screen  Screen to be passed
     */
    void pushScreen(std::unique_ptr<Screen>&& screen);

public:
    // Rendering methods
    std::vector<WidgetInput> widgetVtxBuffer;

    /**
     * Pushes a new screen to the back of the screen array
     * @param[in]  screen  Widget vertices to include in the buffer
     */
    void renderWidget(const WidgetInput& input);
};

}  // namespace ui
