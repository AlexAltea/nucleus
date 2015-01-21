/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "nucleus/filesystem/virtual_filesystem.h"
#include "nucleus/loader/self.h"
#include "nucleus/syscalls/lv2.h"

// Global emulator object
Emulator nucleus;

bool Emulator::load(const std::string& filepath)
{
    // Initialize everything
    memory.init();
    rsx.init();

    // Create mount points
    const std::string& nucleusPath = getEmulatorPath();
    const std::string& processPath = getProcessPath(filepath);
    devices.push_back(new VirtualFileSystem("/dev_flash", nucleusPath + "dev_flash"));
    devices.push_back(new VirtualFileSystem("/dev_hdd0", nucleusPath + "dev_hdd0"));
    devices.push_back(new VirtualFileSystem("/app_home", processPath));
    devices.push_back(new VirtualFileSystem("/host_root/", ""));
    devices.push_back(new VirtualFileSystem("", ""));

    // Load ELF/SELF file
    SELFLoader self;
    if (!self.open(filepath)) {
        log.error(LOG_COMMON, "Invalid file given.");
        return false;
    }

    self.load_elf(lv2.proc);
    if (self.getMachine() != EM_PPC64) {
        log.error(LOG_COMMON, "Only PPC64 executables are allowed");
        return false;
    }

    // Prepare Thread (this will initialize LV2)
    auto* thread = (PPUThread*)cell.addThread(CELL_THREAD_PPU, self.getEntry());
    thread->start();

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
