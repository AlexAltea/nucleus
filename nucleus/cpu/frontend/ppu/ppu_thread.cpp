/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/config.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"

namespace cpu {
namespace frontend {
namespace ppu {

Thread::Thread(U32 entry)
{
    // Initialize stack of size 0x10000
    m_stackAddr = nucleus.memory(SEG_STACK).alloc(0x10000, 0x100);
    m_stackPointer = m_stackAddr + 0x10000;

    state = std::make_unique<State>();

    const U32 entry_pc = nucleus.memory.read32(entry);
    const U32 entry_rtoc = nucleus.memory.read32(entry+4);

    // Initialize Program Counter
    state->pc = entry_pc;

    // Initialize UISA Registers (TODO: All of this might be wrong)
    state->r[0] = entry_pc;
    state->r[1] = m_stackPointer - 0x200;
    state->r[2] = entry_rtoc;
    state->r[3] = 0;
    state->r[4] = m_stackPointer - 0x80;
    state->r[5] = state->r[4] + 0x10;
    state->r[11] = entry;
    state->r[12] = nucleus.lv2.proc.param.malloc_pagesize;
    state->r[13] = nucleus.memory(SEG_USER_MEMORY).getBaseAddr() + 0x7060; // TLS
    state->cr.CR = 0x22000082;
    state->tb.TBL = 1;
    state->tb.TBU = 1;

    // Arguments passed to sys_initialize_tls on liblv2.sprx's start function
    state->r[7] = 0x0; // TODO
    state->r[8] = 0x0; // TODO
    state->r[9] = 0x0; // TODO
    state->r[10] = 0x90;
}

Thread::~Thread()
{
    // Destroy stack
    nucleus.memory(SEG_STACK).free(m_stackAddr);
}

void Thread::start()
{
    if (m_thread) {
        stop();
    }

    m_thread = new std::thread([&](){
        nucleus.cell.setCurrentThread(this);
        m_status = NUCLEUS_STATUS_RUNNING;

        // Initialize LV2 if necessary
        if (!nucleus.lv2.initialized) {
            nucleus.lv2.init();
        }
        // Otherwise start emulation directly
        else {
            task();
        }
    });
}

void Thread::task()
{
    if (config.ppuTranslator & CPU_TRANSLATOR_INSTRUCTION) {
        while (true) {
            // Handle events
            if (m_event) {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_event == NUCLEUS_EVENT_PAUSE) {
                    m_status = NUCLEUS_STATUS_PAUSED;
                    m_cv.wait(lock, [&]{ return m_event == NUCLEUS_EVENT_RUN; });
                    m_status = NUCLEUS_STATUS_RUNNING;
                }
                if (m_event == NUCLEUS_EVENT_STOP) {
                    break;
                }
                m_event = NUCLEUS_EVENT_NONE;
            }
            // Callback finished
            if (state->pc == 0) {
                break;
            }
            //interpreter->step();
        }
    }
    if (config.ppuTranslator & CPU_TRANSLATOR_BLOCK) {
        for (auto* ppu_segment : nucleus.cell.ppu_segments) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            // TODO: ?
        }
    }
    if (config.ppuTranslator & CPU_TRANSLATOR_FUNCTION) {
        for (auto* ppu_segment : nucleus.cell.ppu_segments) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            auto* function = ppu_segment->addFunction(state->pc);
            function->call();
        }
    }
    if (config.ppuTranslator & CPU_TRANSLATOR_MODULE) {
        for (auto* ppu_segment : nucleus.cell.ppu_segments) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            // TODO: ?
        }
    }
}

void Thread::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_RUN;
    m_cv.notify_one();
}

void Thread::pause()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_PAUSE;
}

void Thread::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_STOP;
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
