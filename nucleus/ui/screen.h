/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <chrono>

class Screen
{
    // Timing
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point time_creation;
    Clock::time_point time_current;

protected:
    // Elapsed time in milliseconds since screen creation
    double dtime = 0.0;

    // Rendered frames at this screen
    unsigned int frame = 0;

public:
    // Tells the UI that this screen should be removed
    bool finished = false;

    Screen();

    // Common tasks before and after rendering, respectively
    void prologue();
    void epilogue();

    // Render the screen widgets
    virtual void render()=0;

    // Update the screen members
    virtual void update()=0;
};
