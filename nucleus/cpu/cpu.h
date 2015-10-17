/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/backend/compiler.h"

#include <mutex>

namespace cpu {

class CPU {
    std::mutex mutex;

protected:
    std::unique_ptr<backend::Compiler> compiler;

public:
    std::vector<Thread*> threads;

    // Constructor
    CPU();

    // Manage threads
    Thread* addThread(ThreadType type);
    void removeThread(Thread* thread);

    // Manage current thread
    Thread* getCurrentThread();
    void setCurrentThread(Thread* thread);

    // Manage execution state of all threads
    void run();
    void pause();
    void stop();
};

}  // namespace cpu
