/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "loader/self.h"
#include "syscalls/lv2.h"

bool Emulator::load(const std::string& filepath)
{
    // Initialize everything
    memory.init();

    // Load title / title_id / etc. ?

    // Load ELF/SELF
    SELFLoader self;
    if (!self.open(filepath)) {
        log.error(LOG_COMMON, "Invalid file given.");
        return false;
    }
    self.load_elf();

    // Initialize LV2
    if (!lv2.init()) {
        log.error(LOG_HLE, "Could not initialize LV2.");
        return false;
    }

    // Prepare Thread
    switch (self.getMachine()) {
    case MACHINE_PPC64:
        cell.addThread(CELL_THREAD_PPU, self.getEntry());
        break;
    default:
        // TODO: Error
        break;
    }

    return true;
}

void Emulator::run()
{
    cell.run();
}

void Emulator::pause()
{
}

void Emulator::stop()
{

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
            log.warning(LOG_COMMON, "Unknown event");
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
