/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "window.h"

#include <string>

#include <Windows.h>

class WindowOpenGL : public Window {
    HDC m_hdc;
    HGLRC m_hrc;

public:
    WindowOpenGL(const std::string& title, int width, int height);

    virtual void init() override;
    virtual void swap_buffers() override;
};
