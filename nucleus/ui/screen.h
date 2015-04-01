/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

class Screen
{
public:
    // Tells the UI that this screen should be removed
    bool finished = false;

    // Number of rendered frames at this screen
    unsigned int frame = 0;

    // Render the screen components
    virtual void render()=0;

    // Update the screen components
    virtual void update()=0;
};
