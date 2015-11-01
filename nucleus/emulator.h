/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>

// Forward declarations
namespace gfx { class IBackend; }
namespace ui  { class UI; }
namespace cpu { class CPU; }
namespace gpu { class GPU; }
namespace sys { class System; }

class Emulator {
    std::mutex m_mutex;
    std::condition_variable m_cv;
    EmulatorEvent m_event;
    EmulatorStatus m_status;

public:
    std::shared_ptr<gfx::IBackend> graphics;
    std::shared_ptr<ui::UI> ui;

    // Hardware & OS
    std::shared_ptr<mem::Memory> memory;
    std::shared_ptr<cpu::CPU> cpu;
    std::shared_ptr<gpu::GPU> gpu;
    std::shared_ptr<sys::System> sys;

    // Constructor
    Emulator();

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
