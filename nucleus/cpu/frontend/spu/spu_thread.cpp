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

void SPUThread::mfcCommand(U32 cmd) {
    const auto& mfc = state->mfc;
    switch (cmd) {
    case MFC_PUT_CMD:
	case MFC_PUTB_CMD:
	case MFC_PUTF_CMD:
	case MFC_PUTR_CMD:
	case MFC_PUTRB_CMD:
	case MFC_PUTRF_CMD:
	case MFC_GET_CMD:
	case MFC_GETB_CMD:
	case MFC_GETF_CMD:
        dmaTransfer(cmd, mfc.eal, mfc.lsa, mfc.size);
        break;

    case MFC_PUTL_CMD:
	case MFC_PUTLB_CMD:
	case MFC_PUTLF_CMD:
	case MFC_PUTRL_CMD:
	case MFC_PUTRLB_CMD:
	case MFC_PUTRLF_CMD:
	case MFC_GETL_CMD:
	case MFC_GETLB_CMD:
	case MFC_GETLF_CMD:
        dmaTransferList(cmd, mfc.eal, mfc.lsa, mfc.size);
        break;

    default:
        assert_always("Unimplemented");
    }
}

void SPUThread::dmaTransfer(U32 cmd, U32 eal, U32 lsa, U32 size) {
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

void SPUThread::dmaTransferList(U32 cmd, U32 eal, U32 lsa, U32 size) {
    const U32 listAddr = eal & 0x3ffff;
	const U32 listSize = size / 8;

    const auto& memory = parent->memory;
    const auto* list = memory->ptr<MFCListElement>(listAddr);

    for (Size i = 0; i < listSize; i++) {
        const auto& entry = list[i];
        if (entry.lts) {
            dmaTransfer(cmd, entry.leal, lsa, entry.lts);
        }
        if (entry.s) {
            assert_always("Unimplemented");
        }
    }
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
