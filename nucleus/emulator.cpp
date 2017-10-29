/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "nucleus/core/config.h"
#include "nucleus/filesystem/filesystem_host.h"
#include "nucleus/filesystem/filesystem_virtual.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/cpu/cpu_host.h"
#include "nucleus/ui/ui.h"
#include "nucleus/gpu/list.h"
#include "nucleus/logger/logger.h"
#include "nucleus/memory/list.h"
#include "nucleus/system/loader.h"
#include "nucleus/system/list.h"

#if !defined(NUCLEUS_BUILD_TEST)

/**
 * Load specific platform
 */
bool Emulator::load_ps3(const std::string& path) {
    memory = std::make_shared<mem::GuestVirtualMemory>(4_GB);
    cpu = std::make_shared<cpu::GuestCPU>(this, memory.get());
    gpu = std::make_shared<gpu::RSX>(this, memory.get(), graphics);
    sys = std::make_shared<sys::LV2>(this, memory, sys::LV2_DEX);
    return sys->start(path);
}

bool Emulator::load_ps4(const std::string& path) {
    memory = std::make_shared<mem::HostVirtualMemory>();
    cpu = std::make_shared<cpu::HostCPU>(this, memory.get());
    gpu = std::make_shared<gpu::R10XX>(this, graphics);
    sys = std::make_shared<sys::OrbisOS>(this);
    return sys->start(path);
}

bool Emulator::load(const std::string& filepath) {
    switch (detectPlatform(filepath)) {
    case core::PLATFORM_PS3:
        return load_ps3(filepath);
    case core::PLATFORM_PS4:
        return load_ps4(filepath);
    default:
        logger.error(LOG_COMMON, "Unsupported platform");
        return false;
    }
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

#endif  // !defined(NUCLEUS_BUILD_TEST)
