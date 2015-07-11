/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/filesystem/filesystem.h"
#include "nucleus/memory/memory.h"
#include "nucleus/gpu/rsx.h"
#include "nucleus/system/lv2.h"

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
    cpu::Cell cell;
    gpu::RSX rsx;
    sys::LV2 lv2;

    Emulator() : lv2(sys::LV2_DEX) {}

    // Control the emulated process
    bool load(const std::string& filepath);
    void run();
    void pause();
    void stop();

    // Wait for events
    void idle();
    void task(EmulatorEvent evt);
};

extern Emulator nucleus;
