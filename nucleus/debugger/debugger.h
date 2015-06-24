/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "externals/mongoose/mongoose.h"

#include <thread>

class Debugger
{
    // Mongoose server
    mg_server* m_server = nullptr;

    // Listener thread
    std::thread* m_listener;

public:
    ~Debugger() { exit(); }

    // Start a new listener thread
    bool init();

    // Wait for new connections
    void listen();

    // Destroy the Mongoose server
    bool exit();
};

extern Debugger debugger;
