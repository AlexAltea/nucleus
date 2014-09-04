/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cell.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

CellThread& Cell::addThread(CellThreadType type, u32 entry=0)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    CellThread* thread;

    switch (type)
    {
    case CELL_THREAD_PPU:
        thread = new PPUThread();
        thread->start(entry);
        break;
    default:
        // TODO: Error
        break;
    }

    m_threads.push_back(thread);
    return *thread;
}

void Cell::run()
{
    for (CellThread* thread : m_threads) {
        thread->run();
    }
}

void Cell::pause()
{
    for (CellThread* thread : m_threads) {
        thread->pause();
    }
}
