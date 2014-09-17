/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/syscalls/lv2.h"

#include <mutex>
#include <condition_variable>
#include <string>

class Emulator
{
    std::mutex m_mutex;
    std::condition_variable m_cv;
    EmulatorEvent m_event;
    EmulatorStatus m_status;

public:
    // Hardware & OS
    Memory memory;
    Cell cell;
    LV2 lv2;

    // Control the emulated process
    bool load(const std::string& filepath);
    void run();
    void pause();
    void stop();

    // Wait for events
    void idle();

    // Logging
    void log(const char* format, ...);
};

extern Emulator nucleus;
