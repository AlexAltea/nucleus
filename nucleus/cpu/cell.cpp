/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cell.h"
#include "nucleus/config.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_thread.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

#include <algorithm>

#ifdef NUCLEUS_PLATFORM_WINDOWS
#define thread_local __declspec(thread)
#elif NUCLEUS_PLATFORM_OSX
#define thread_local __thread
#endif

namespace cpu {

thread_local CellThread* g_this_thread = nullptr;

Cell::Cell()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

void Cell::init()
{
    if (config.ppuTranslator == PPU_TRANSLATOR_RECOMPILER) {
        // Global target triple
        llvm::Triple triple(llvm::sys::getProcessTriple());
        if (triple.getOS() == llvm::Triple::OSType::Win32) {
            triple.setObjectFormat(llvm::Triple::ObjectFormatType::ELF);
        }

        // Global Nucleus module
        module = new llvm::Module("Nucleus", llvm::getGlobalContext());
        module->setTargetTriple(triple.str());

        // Global variables
        module->getOrInsertGlobal("memoryBase", llvm::Type::getInt64Ty(llvm::getGlobalContext()));
        llvm::GlobalVariable* memoryBase = module->getNamedGlobal("memoryBase");
        memoryBase->setConstant(true);
        memoryBase->setLinkage(llvm::GlobalValue::ExternalLinkage);
        memoryBase->setInitializer(llvm::ConstantInt::get(module->getContext(), llvm::APInt(64, (u64)nucleus.memory.getBaseAddr())));
    }
}

CellThread* Cell::addThread(CellThreadType type, u32 entry=0)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    CellThread* thread;

    switch (type) {
    case CELL_THREAD_PPU:
        thread = new ppu::Thread(entry);
        ppu_threads.push_back(dynamic_cast<ppu::Thread*>(thread));
        break;

    case CELL_THREAD_SPU:
        nucleus.log.error(LOG_CPU, "Unimplemented Cell thread type");
        return nullptr;

    case CELL_THREAD_RAWSPU:
        nucleus.log.error(LOG_CPU, "Unimplemented Cell thread type");
        return nullptr;

    default:
        nucleus.log.error(LOG_CPU, "Unsupported Cell thread type");
        return nullptr;
    }

    // Give the thread a unused ID
    while (m_thread_ids.find(m_current_id) != m_thread_ids.end()) {
        m_current_id += 1;
    }
    thread->id = m_current_id;
    m_thread_ids.insert(m_current_id);
    m_current_id += 1;

    return thread;
}

CellThread* Cell::getThread(u64 id)
{
    for (CellThread* thread : ppu_threads) {
        if (thread->id == id) {
            return thread;
        }
    }
}

void Cell::removeThread(u64 id)
{
    // Remove thread
    ppu_threads.erase(
        std::remove_if(ppu_threads.begin(), ppu_threads.end(), [&](CellThread* thread){ return thread->id == id; }),
        ppu_threads.end());

    // Remove thread ID
    m_thread_ids.erase(id);
}

CellThread* Cell::getCurrentThread()
{
    return g_this_thread;
}

void Cell::setCurrentThread(CellThread* thread)
{
    g_this_thread = thread;
}

void Cell::run()
{
    for (CellThread* thread : ppu_threads) {
        thread->run();
    }
}

void Cell::pause()
{
    for (CellThread* thread : ppu_threads) {
        thread->pause();
    }
}

void Cell::stop()
{
    for (CellThread* thread : ppu_threads) {
        thread->stop();
    }
}

}  // namespace cpu
