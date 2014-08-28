/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "memory/memory.h"

#include <string>

class Emulator
{

public:
    Memory memory;

    bool load(const std::string& filepath);
    void run();
    void pause();
    void stop();
};

extern Emulator nucleus;
