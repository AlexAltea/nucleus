/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_thread.h"
#include "nucleus/core/config.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/frontend/spu/spu_decoder.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace spu {

SPUThread::SPUThread(CPU* parent) : Thread(parent) {
    state = std::make_unique<SPUState>();
}

void SPUThread::start() {
    m_thread = std::thread([&](){
        parent->setCurrentThread(this);
        task();
    });
}

void SPUThread::task() {
     if (config.spuTranslator & CPU_TRANSLATOR_FUNCTION) {
        for (auto* spu_segment : static_cast<Cell*>(parent)->spu_modules) {
            if (!spu_segment->contains(state->pc)) {
                continue;
            }

            auto* function = spu_segment->addFunction(state->pc);
            auto* hirFunction = function->hirFunction;
            if (!(hirFunction->flags & hir::FUNCTION_IS_COMPILED)) {
                parent->compiler->compile(hirFunction);
            }
            parent->compiler->call(hirFunction, state.get());
            return;
        }
    }
}

void SPUThread::run() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_RUN;
    m_cv.notify_one();
}

void SPUThread::pause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_PAUSE;
}

void SPUThread::stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_STOP;
}

/*void SPUThread::dmaTransfer(U32 cmd, U32 eal, U32 lsa, U32 size) {
    if (cmd & (MFC_BARRIER_ENABLE | MFC_FENCE_ENABLE)) {
#ifdef NUCLEUS_ARCH_X86
        _mm_mfence();
#endif
    }

    const auto& memory = parent->memory;
    switch (cmd & ~(MFC_BARRIER_ENABLE | MFC_FENCE_ENABLE)) {
    case MFC_PUT_CMD:
    case MFC_PUTR_CMD:
        memcpy(memory->ptr(eal), memory->ptr(lsa), size);
        break;
    case MFC_GET_CMD:
        memcpy(memory->ptr(lsa), memory->ptr(eal), size);
        break;
    default:
        assert_true("Unexpected");
    }
}

void dmaTransferList() {
}*/

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
