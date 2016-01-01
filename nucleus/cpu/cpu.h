/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/backend/compiler.h"

#include <mutex>

namespace cpu {

class CPU {
    std::mutex mutex;

public:
    std::shared_ptr<mem::Memory> memory;

    std::unique_ptr<backend::Compiler> compiler;

    std::vector<Thread*> threads;

    // Constructor
    CPU(std::shared_ptr<mem::Memory> memory);

    // Manage threads
    Thread* addThread(ThreadType type);
    void removeThread(Thread* thread);

    // Manage current thread
    static Thread* getCurrentThread();
    static void setCurrentThread(Thread* thread);

    // Manage execution state of all threads
    void run();
    void pause();
    void stop();
};

}  // namespace cpu
