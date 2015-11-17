/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/widgets/list.h"

#include <chrono>

namespace ui {

// Forward declarations
class UI;

class Screen {
    // Timing
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point time_creation;
    Clock::time_point time_current;

protected:
    UI* parent;
    WidgetContainer body;

    // Elapsed time since screen creation
    Clock::duration dtime;

    // Rendered frames at this screen
    unsigned int frame = 0;

public:
    // Tells the UI that this screen should be removed
    bool finished = false;

    Screen(UI* parent);

    // Common tasks before and after rendering, respectively
    void prologue();
    void epilogue();

    // Render the screen widgets
    void render(gfx::CommandBuffer* cmdBuffer);

    // Update the screen members
    virtual void update() = 0;
};

}  // namespace ui
