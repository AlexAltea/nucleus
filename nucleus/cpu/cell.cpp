/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cell.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

#ifdef NUCLEUS_WIN
#define thread_local __declspec(thread)
#elif NUCLEUS_MACOS
#define thread_local __thread
#endif

thread_local CellThread* g_this_thread = nullptr;

CellThread& Cell::addThread(CellThreadType type, u32 entry=0)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    CellThread* thread;

    switch (type)
    {
    case CELL_THREAD_PPU:
        thread = new PPUThread(entry);
        thread->start();
        break;
    default:
        // TODO: Error
        break;
    }

    m_threads.push_back(thread);
    return *thread;
}

CellThread* Cell::getCurrentThread()
{
    return g_this_thread;
}

void Cell::setCurrentThread(CellThread* thread)
{
    g_this_thread = thread;
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

void Cell::stop()
{
    for (CellThread* thread : m_threads) {
        thread->stop();
    }
}
