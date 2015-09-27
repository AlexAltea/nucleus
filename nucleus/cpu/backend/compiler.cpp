/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "compiler.h"
#include "nucleus/logger/logger.h"

#ifdef NUCLEUS_PLATFORM_WINDOWS
#include <Windows.h>
#endif
#ifdef NUCLEUS_PLATFORM_LINUX
#include <sys/mman.h>
#endif
#ifdef NUCLEUS_PLATFORM_OSX
#include <sys/mman.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace cpu {
namespace backend {

using namespace cpu::hir;

Compiler::Compiler() {
    std::memset(&targetInfo, 0, sizeof(targetInfo));

    // Default compiler settings
    settings.isCached = true;
    settings.isJIT = true;
    settings.isAOT = false;
}

Compiler::Compiler(const Settings& settings) : settings(settings) {
    std::memset(&targetInfo, 0, sizeof(targetInfo));
}

bool Compiler::optimize(Function* function) {
    for (auto& pass : passes) {
        if (!pass->run(function)) {
            logger.error(LOG_CPU, "Could not run pass: %s", pass->name());
            return false;
        }
    }
    return true;
}

void Compiler::addPass(std::unique_ptr<Pass> pass) {
    passes.push_back(std::move(pass));
}

void* Compiler::allocRWXMemory(size_t size) {
    void* addr = malloc(size);

    // Enable execution permissions
#ifdef NUCLEUS_PLATFORM_WINDOWS
    DWORD oldProtect;
    if (!VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        logger.error(LOG_CPU, "Could not allocate %d bytes of RWX memory", size);
        return nullptr;
    }
#endif
#ifdef NUCLEUS_PLATFORM_LINUX
    size_t pageSize = sysconf(_SC_PAGESIZE);
    size_t iaddr = reinterpret_cast<size_t>(addr);
	size_t roundedAddr = iaddr & ~(pageSize - 1);
    if (!mprotect(reinterpret_cast<void*>(roundAddr), size + (iaddr - roundAddr), mode)) {
        logger.error(LOG_CPU, "Could not allocate %d bytes of RWX memory", size);
        return nullptr;
    }
#endif
    return addr;
}

void Compiler::freeRWXMemory(void* addr) {
    free(addr);
}

}  // namespace backend
}  // namespace cpu
