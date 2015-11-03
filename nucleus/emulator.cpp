/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "nucleus/core/config.h"
#include "nucleus/graphics/backends.h"
#include "nucleus/ui/ui.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/gpu/rsx/rsx.h"
#include "nucleus/filesystem/utils.h"
#include "nucleus/loader/self.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/lv2.h"

// Global emulator object
Emulator nucleus;

bool Emulator::initialize(const gfx::BackendParameters& params) {
    switch (config.graphicsBackend) {
    case GRAPHICS_BACKEND_OPENGL:
        graphics = std::make_shared<gfx::OpenGLBackend>();
        break;
    case GRAPHICS_BACKEND_DIRECT3D:
        graphics = std::make_shared<gfx::Direct3D12Backend>();
        break;
    default:
        logger.warning(LOG_COMMON, "Unsupported graphics backend");
        return false;
    }

    if (!graphics->initialize(params)) {
        logger.warning(LOG_COMMON, "Could not initialize graphics backend");
        return false;
    }

    if (!config.console) {
        ui = std::make_shared<ui::UI>(graphics);
        if (!ui->initialize()) {
            logger.warning(LOG_COMMON, "Could not initialize user interface");
            return false;
        }
    }

    return true;
}

bool Emulator::load(const std::string& filepath) {
    // Initialize hardware
    memory = std::make_shared<mem::Memory>();
    cpu = std::make_shared<cpu::Cell>(memory);
    gpu = std::make_shared<gpu::RSX>(memory, graphics);
    sys = std::make_shared<sys::LV2>(memory, sys::LV2_DEX);

    // Initialize application filesystem devices
    const fs::Path& processPath = fs::getProcessPath(filepath);
    sys->vfs.registerDevice(new fs::HostPathDevice("/app_home/", processPath));

    // Load ELF/SELF file
    SELFLoader self;
    auto file = fs::HostFileSystem::openFile(filepath, fs::Read);
    if (!self.open(file.get())) {
        logger.error(LOG_COMMON, "Invalid file given.");
        return false;
    }

    self.load_elf(static_cast<sys::LV2*>(sys.get())->proc);
    if (self.getMachine() != EM_PPC64) {
        logger.error(LOG_COMMON, "Only PPC64 executables are allowed");
        return false;
    }

    auto entry = self.getEntry();
    static_cast<sys::LV2*>(sys.get())->init(entry);
    return true;
}

void Emulator::run() {
    cpu->run();
}

void Emulator::pause() {
    cpu->pause();
}

void Emulator::stop() {
    cpu->stop();
}

void Emulator::idle() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]{ return m_event; });

        // Process event
        switch (m_event) {
        case NUCLEUS_EVENT_RUN:
            cpu->run();
            break;
        case NUCLEUS_EVENT_PAUSE:
            cpu->pause();
            break;
        case NUCLEUS_EVENT_STOP:
            cpu->stop();
            return;
        case NUCLEUS_EVENT_CLOSE:
            return;
        default:
            logger.warning(LOG_COMMON, "Unknown event");
            break;
        }

        m_event = NUCLEUS_EVENT_NONE;
    }
}

void Emulator::task(EmulatorEvent evt) {
    m_event = evt;
    m_cv.notify_one();
}
