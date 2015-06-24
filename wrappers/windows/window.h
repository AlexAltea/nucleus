/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

#include <Windows.h>

class Window {
protected:
    HWND m_hwnd;

    std::string m_title;
    int m_width;
    int m_height;

public:
    Window(const std::string& title, int width, int height);

    void loop();

    virtual void connect_ui()=0;
    virtual void connect_rsx()=0;
    virtual void swap_buffers()=0;
};
