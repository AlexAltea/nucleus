/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "nucleus/filesystem/utils.h"
#include "nucleus/loader/self.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/lv2.h"

// Global emulator object
Emulator nucleus;

bool Emulator::load(const std::string& filepath)
{
    // Initialize hardware
    memory.init();
    cell.init();
    rsx.init();

    // Initialize application filesystem devices
    const fs::Path& processPath = fs::getProcessPath(filepath);
    lv2.vfs.registerDevice(new fs::HostPathDevice("/app_home", processPath));

    // Load ELF/SELF file
    SELFLoader self;
    if (!self.open(filepath)) {
        logger.error(LOG_COMMON, "Invalid file given.");
        return false;
    }

    self.load_elf(lv2.proc);
    if (self.getMachine() != EM_PPC64) {
        logger.error(LOG_COMMON, "Only PPC64 executables are allowed");
        return false;
    }

    // Prepare Thread (this will initialize LV2)
    auto* thread = cell.addThread(CELL_THREAD_PPU, self.getEntry());
    thread->start();

    return true;
}

void Emulator::run()
{
    cell.run();
}

void Emulator::pause()
{
    cell.pause();
}

void Emulator::stop()
{
    cell.stop();
}

void Emulator::idle()
{
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]{ return m_event; });

        // Process event
        switch (m_event) {
        case NUCLEUS_EVENT_RUN:
            cell.run();
            break;
        case NUCLEUS_EVENT_PAUSE:
            cell.pause();
            break;
        case NUCLEUS_EVENT_STOP:
            cell.stop();
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

void Emulator::task(EmulatorEvent evt)
{
    m_event = evt;
    m_cv.notify_one();
}
