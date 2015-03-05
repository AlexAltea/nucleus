/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/config.h"

// Global UI manager object
UI ui;

void UI::init()
{
    // Initialize UI translation mechanisms
    language.open(config.language);

    m_window_thread = new std::thread([&](){
        task();
    });
}

void UI::task()
{
}
