/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cpu.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/hir/passes.h"
#include "nucleus/logger/logger.h"

// Backends
#include "nucleus/cpu/backend/x86/x86_compiler.h"

// Frontends
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"

#include <algorithm>

namespace cpu {

thread_local Thread* gCurrentThread = nullptr;

CPU::CPU() {
#if defined(NUCLEUS_ARCH_X86)
    compiler = std::make_unique<backend::x86::X86Compiler>();
#elif defined(NUCLEUS_ARCH_ARM)
    compiler = std::make_unique<backend::arm::ARMCompiler>();
#else
    logger.error(LOG_CPU, "No backend available for this architecture.");
#endif

    // Compiler passes
    compiler->addPass(std::make_unique<hir::passes::RegisterAllocationPass>(compiler->targetInfo));
}

Thread* CPU::addThread(ThreadType type) {
    std::lock_guard<std::mutex> lock(mutex);

    Thread* thread;
    switch (type) {
    case THREAD_TYPE_PPU:
        thread = new frontend::ppu::PPUThread(this);
        break;
    case THREAD_TYPE_SPU:
        thread = new frontend::spu::SPUThread(this);
        return nullptr;
    case THREAD_TYPE_RAWSPU:
        logger.error(LOG_CPU, "Unimplemented CPU thread type");
        return nullptr;

    default:
        logger.error(LOG_CPU, "Unsupported CPU thread type");
        return nullptr;
    }

    threads.push_back(thread);
    return thread;
}

void CPU::removeThread(Thread* thread) {
    std::lock_guard<std::mutex> lock(mutex);

    threads.erase(
        std::remove_if(threads.begin(), threads.end(), [&](Thread* other){ return thread == other; }),
        threads.end());
}

Thread* CPU::getCurrentThread() {
    return gCurrentThread;
}

void CPU::setCurrentThread(Thread* thread) {
    gCurrentThread = thread;
}

void CPU::run() {
    std::lock_guard<std::mutex> lock(mutex);

    for (Thread* thread : threads) {
        thread->run();
    }
}

void CPU::pause() {
    std::lock_guard<std::mutex> lock(mutex);

    for (Thread* thread : threads) {
        thread->pause();
    }
}

void CPU::stop() {
    std::lock_guard<std::mutex> lock(mutex);

    for (Thread* thread : threads) {
        thread->stop();
    }
}

}  // namespace cpu
