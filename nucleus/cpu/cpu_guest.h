/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/backend/compiler.h"

#include <mutex>

namespace cpu {

class GuestCPU : public CPU {
    std::mutex mutex;

public:
    mem::Memory* memory;

    std::unique_ptr<backend::Compiler> compiler;

    std::vector<Thread*> threads;

    // Constructor
    GuestCPU(mem::Memory* memory);

    // Manage threads
    Thread* addThread(ThreadType type);
    void removeThread(Thread* thread);

    // Manage current thread
    static Thread* getCurrentThread();
    static void setCurrentThread(Thread* thread);

    // Thread management
    virtual void run() override;
    virtual void pause() override;
    virtual void stop() override;
};

}  // namespace cpu
