/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/widgets/widget_container.h"

#include <chrono>

using namespace std::literals::chrono_literals;

namespace ui {

// Forward declarations
class UI;

/**
 * Screen
 * ======
 * Represents the document containing the Widget tree to be rendered.
 *
 * ## Rendering
 * Consist of multiple stages described below:
 *  1. The widget tree is traversed bottom-up, computing the Widget sizes based on their own style
 *     and the dimensions computed in the child nodes. Relevant style properties are:
 *     Width, Height, Margin, Padding
 *  2. The widget tree is traversed up-bottom, doing following operations:
 *      a. Computing the Widget positions relative to the window coordinates based on their own style
 *         and the positions computed in the parent node. Relevant style properties are:
 *         Top, Left, Bottom, Right
 *      b. Verify whether the Widget is visible on the screen with the computed size and position.
 *         If it is, generate and append its vertex attributes to the global UI vertex buffer.
 *  3. Render the generated vertex buffer.
 */
class Screen {
    // Timing
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point time_creation;
    Clock::time_point time_current;

protected:
    UI* manager;
    WidgetContainer body;

    // Elapsed time since screen creation
    Clock::duration dtime;

    // Rendered frames at this screen
    unsigned int frame = 0;

public:
    // Tells the UI that this screen should be removed
    bool finished = false;

    Screen(UI* ui);

    // Common tasks before and after rendering, respectively
    void prologue();
    void epilogue();

    // Render the screen widgets
    void render();

    /**
     * Handles an event, propagating it from parents to children (capture model)
     * @param[in]  evt  Event to handle
     */
    void handle(Event& evt);

    // Update the screen members
    virtual void update() = 0;
};

}  // namespace ui
