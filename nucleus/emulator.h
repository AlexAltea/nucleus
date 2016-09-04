/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/core/host.h"
#include "nucleus/audio/audio.h"
#include "nucleus/graphics/graphics.h"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>

// Forward declarations
namespace ui  { class UI; }
namespace cpu { class CPU; }
namespace gpu { class GPU; }
namespace mem { class Memory; }
namespace sys { class System; }

class Emulator {
    std::mutex m_mutex;
    std::condition_variable m_cv;
    EmulatorEvent m_event;
    EmulatorStatus m_status;

    // Load specific platform
    bool load_ps3(const std::string& path);
    bool load_ps4(const std::string& path);

public:
    std::shared_ptr<audio::AudioBackend> audio;
    std::shared_ptr<gfx::GraphicsBackend> graphics;
    std::shared_ptr<ui::UI> ui;

    // Hardware & OS
    std::shared_ptr<mem::Memory> memory;
    std::shared_ptr<cpu::CPU> cpu;
    std::shared_ptr<gpu::GPU> gpu;
    std::shared_ptr<sys::System> sys;

    // Initializes the emulator
    bool initialize(const gfx::BackendParameters& params);

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
