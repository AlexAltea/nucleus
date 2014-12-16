/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

class Window {
protected:
    std::string m_title;
    int m_width;
    int m_height;

public:
    Window(const std::string& title, int width, int height);

    void loop();

    virtual void init()=0;
    virtual void swap_buffers()=0;
};
