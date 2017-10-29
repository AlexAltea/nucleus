/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/backend/compiler.h"

#include <mutex>

// Forward declarations
namespace cpu::frontend::ppu { class Module; }
namespace cpu::frontend::spu { class Module; }

namespace cpu {

class GuestCPU : public CPU {
    std::mutex mutex;

public:
    std::unique_ptr<backend::Compiler> compiler;

    std::vector<Thread*> threads;

    std::vector<frontend::ppu::Module*> ppu_modules;
    std::vector<frontend::spu::Module*> spu_modules;

    // Constructor
    GuestCPU(Emulator* emulator, mem::Memory* memory);

    // Manage threads
    Thread* addThread(ThreadType type);
    void removeThread(Thread* thread);

    // Thread management
    virtual void run() override;
    virtual void pause() override;
    virtual void stop() override;
};

}  // namespace cpu
