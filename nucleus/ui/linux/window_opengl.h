/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "window.h"

#include <string>

class WindowOpenGL : public Window {

public:
    WindowOpenGL(const std::string& title, int width, int height);

    virtual void init() override;
    virtual void swap_buffers() override;
};
