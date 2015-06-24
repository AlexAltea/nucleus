/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "window.h"

#include <string>

#include <Windows.h>

class WindowOpenGL : public Window {
    HDC hdc;
    HGLRC hrc_ui;
    HGLRC hrc_rsx;

public:
    WindowOpenGL(const std::string& title, int width, int height);

    virtual void connect_ui() override;
    virtual void connect_rsx() override;
    virtual void swap_buffers() override;
};
