/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include <mutex>
#include <vector>

enum CellThreadType
{
    CELL_THREAD_PPU,
    CELL_THREAD_SPU,
    CELL_THREAD_RAWSPU,
};

class Cell
{
    std::vector<CellThread*> m_threads;
    std::mutex m_mutex;

public:
    CellThread& addThread(CellThreadType type, u32 entry);
    void removeThread(u32 id);

    void run();
    void pause();
    void stop();
};
