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

PPUThread::PPUThread() {
    state = std::make_unique<State>();

    // Initialize UISA Registers
    state->cr.CR = 0x22000082;
    state->fpscr.FPSCR = 0x00002000; // TODO
    state->tb.TBL = 1;
    state->tb.TBU = 1;

    // Arguments passed to sys_initialize_tls on liblv2.sprx's start function
    state->r[7] = 0x0; // TODO
    state->r[8] = 0x0; // TODO
    state->r[9] = 0x0; // TODO
    state->r[10] = 0x90;
}

void PPUThread::start() {
    m_thread = std::thread([&](){
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

void PPUThread::task()
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
        for (auto* ppu_segment : parent->ppu_segments) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            auto* function = ppu_segment->addFunction(state->pc);
            function->call();
            return;
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

void PPUThread::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_RUN;
    m_cv.notify_one();
}

void PPUThread::pause()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_PAUSE;
}

void PPUThread::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_STOP;
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
