/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"

#include <mutex>
#include <vector>
#include <set>

namespace cpu {

class Cell : public CPU {
    std::mutex m_mutex;

    // Thread ID information
    std::set<U64> m_thread_ids;
    U64 m_current_id = 1;

public:
    // Cell threads
    std::vector<frontend::ppu::Thread*> ppu_threads;

    // Executable memory segments
    std::vector<frontend::ppu::Module*> ppu_segments;

    void init();

    // Thread management
    Thread* addThread(ThreadType type, U32 entry);
    Thread* getThread(U64 id);
    void removeThread(U64 id);

    // Current thread ID
    Thread* getCurrentThread();
    void setCurrentThread(Thread* thread);

    // Control CPU state
    void run();
    void pause();
    void stop();
};

}  // namespace cpu
