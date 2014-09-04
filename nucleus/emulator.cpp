/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "loader/self.h"

#include <iostream>

bool Emulator::load(const std::string& filepath)
{
    // Initialize everything
    memory.init();

    // Load title / title_id / etc. ?

    // Load ELF/SELF
    SELFLoader self;
    if (!self.open(filepath)) {
        std::cerr << "Invalid file given." << std::endl;
        return false;
    }
    self.load();

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
            break;
        case NUCLEUS_EVENT_PAUSE:
            break;
        case NUCLEUS_EVENT_STOP:
            break;
        case NUCLEUS_EVENT_CLOSE:
            return;
        default:
            // TODO: Error
            break;
        }

        m_event = NUCLEUS_EVENT_NONE;
    }
}
