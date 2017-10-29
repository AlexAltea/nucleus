/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/core/config.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"

namespace cpu {
namespace frontend {
namespace ppu {

PPUThread::PPUThread(CPU* parent) : Thread(parent) {
    state = std::make_unique<PPUState>();
}

void PPUThread::start() {
    m_thread = std::thread([&](){
        parent->setCurrentThread(this);
        m_status = NUCLEUS_STATUS_RUNNING;
        task();
    });
}

void PPUThread::task()
{
    auto* cpu = dynamic_cast<GuestCPU*>(parent);

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
        for (auto* ppu_segment : cpu->ppu_modules) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            // TODO: ?
        }
    }

    if (config.ppuTranslator & CPU_TRANSLATOR_FUNCTION) {
        for (auto* ppu_segment : cpu->ppu_modules) {
            if (!ppu_segment->contains(state->pc)) {
                continue;
            }

            auto* function = ppu_segment->addFunction(state->pc);
            auto* hirFunction = function->hirFunction;
            if (!(hirFunction->flags & hir::FUNCTION_IS_COMPILED)) {
                cpu->compiler->compile(hirFunction);
            }
            cpu->compiler->call(hirFunction, state.get());
            return;
        }
    }

    if (config.ppuTranslator & CPU_TRANSLATOR_MODULE) {
        for (auto* ppu_segment : cpu->ppu_modules) {
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
