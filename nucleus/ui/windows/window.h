/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

#include <Windows.h>

// Menu entry IDs
enum {
    UI_FILE_EXIT,
    UI_HELP_ABOUT,
};

class Window {
    HWND m_hwnd;
    HMENU m_hmenu;

    std::string m_title;
    int m_width;
    int m_height;

public:
    Window(const std::string& title, int width, int height);

    bool init();

    void loop();
};
