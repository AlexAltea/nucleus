/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

#include "nucleus/cpu/ppu/ppu_decoder.h"

#include <mutex>
#include <vector>
#include <set>

namespace cpu {

class Cell
{
    std::mutex m_mutex;

    // Thread ID information
    std::set<u64> m_thread_ids;
    u64 m_current_id = 1;

public:
    // Cell threads
    std::vector<ppu::Thread*> ppu_threads;

    // Executable memory segments
    std::vector<ppu::Segment*> ppu_segments;

    Cell();

    void init();

    // Recompiler utilities
    llvm::Module* module;
    llvm::ExecutionEngine* executionEngine;

    // Thread management
    CellThread* addThread(CellThreadType type, u32 entry);
    CellThread* getThread(u64 id);
    void removeThread(u64 id);

    // Current thread ID
    CellThread* getCurrentThread();
    void setCurrentThread(CellThread* thread);

    // Control CPU state
    void run();
    void pause();
    void stop();
};

}  // namespace cpu
