/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/compiler.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"

#include <mutex>
#include <vector>
#include <set>

namespace cpu {

class Cell {
    std::mutex m_mutex;

    // Thread ID information
    std::set<U64> m_thread_ids;
    U64 m_current_id = 1;

public:
    std::unique_ptr<backend::Compiler> compiler;

    // Cell threads
    std::vector<ppu::Thread*> ppu_threads;

    // Executable memory segments
    std::vector<ppu::Segment*> ppu_segments;

    Cell();

    void init();

    // Recompiler utilities
    llvm::Module* module;

    // Thread management
    CellThread* addThread(CellThreadType type, U32 entry);
    CellThread* getThread(U64 id);
    void removeThread(U64 id);

    // Current thread ID
    CellThread* getCurrentThread();
    void setCurrentThread(CellThread* thread);

    // Control CPU state
    void run();
    void pause();
    void stop();
};

}  // namespace cpu
