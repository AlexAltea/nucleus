/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

#include <Windows.h>

class Window {
private:
    HWND hwnd;

    std::string title;
    int width;
    int height;

public:
    HDC hdc;

    Window(const std::string& title, int width, int height);

    void loop();

    void swapBuffers();
};
