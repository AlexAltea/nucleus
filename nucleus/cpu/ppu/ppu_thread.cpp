/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/config.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"

namespace cpu {
namespace ppu {

Thread::Thread(u32 entry)
{
    // Initialize stack of size 0x10000
    m_stackAddr = nucleus.memory(SEG_STACK).alloc(0x10000, 0x100);
    m_stackPointer = m_stackAddr + 0x10000;

    if (config.ppuTranslator == PPU_TRANSLATOR_INTERPRETER) {
        interpreter = new Interpreter(entry, m_stackPointer);
        state = &(interpreter->state);
    }
    if (config.ppuTranslator == PPU_TRANSLATOR_RECOMPILER) {
        // TODO
    }
}

Thread::~Thread()
{
    // Destroy stack
    nucleus.memory(SEG_STACK).free(m_stackAddr);

    // Delete translators
    delete interpreter;
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
    if (config.ppuTranslator == PPU_TRANSLATOR_INTERPRETER) {
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
            interpreter->step();
        }
    }
    if (config.ppuTranslator == PPU_TRANSLATOR_RECOMPILER) {
        // TODO
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
}  // namespace cpu
