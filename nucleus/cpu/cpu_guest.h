/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "cpu.h"

namespace cpu {

class GuestCPU : public CPU {
    std::mutex mutex;

public:
    std::shared_ptr<mem::Memory> memory;

    std::unique_ptr<backend::Compiler> compiler;

    std::vector<Thread*> threads;

    // Constructor
    GuestCPU(std::shared_ptr<mem::Memory> memory);

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
