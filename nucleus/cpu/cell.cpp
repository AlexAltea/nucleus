/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cell.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

#include <algorithm>

#ifdef NUCLEUS_WIN
#define thread_local __declspec(thread)
#elif NUCLEUS_MACOS
#define thread_local __thread
#endif

thread_local CellThread* g_this_thread = nullptr;

CellThread* Cell::addThread(CellThreadType type, u32 entry=0)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    CellThread* thread;

    switch (type) {
    case CELL_THREAD_PPU:
        thread = new PPUThread(entry);
        break;
    case CELL_THREAD_SPU:
        nucleus.log.error(LOG_CPU, "Unimplemented Cell thread type");
        return nullptr;
    case CELL_THREAD_RAWSPU:
        nucleus.log.error(LOG_CPU, "Unimplemented Cell thread type");
        return nullptr;

    default:
        nucleus.log.error(LOG_CPU, "Unsupported Cell thread type");
        return nullptr;
    }

    // Give the thread a unused ID
    while (m_thread_ids.find(m_current_id) != m_thread_ids.end()) {
        m_current_id += 1;
    }
    thread->id = m_current_id;
    m_thread_ids.insert(m_current_id);
    m_current_id += 1;

    // Store and return the thread
    m_threads.push_back(thread);
    return thread;
}

CellThread* Cell::getThread(u64 id)
{
    for (CellThread* thread : m_threads) {
        if (thread->id == id) {
            return thread;
        }
    }
}

void Cell::removeThread(u64 id)
{
    // Remove thread
    m_threads.erase(
        std::remove_if(m_threads.begin(), m_threads.end(), [&](CellThread* thread){ return thread->id == id; }),
        m_threads.end());

    // Remove thread ID
    m_thread_ids.erase(id);
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
