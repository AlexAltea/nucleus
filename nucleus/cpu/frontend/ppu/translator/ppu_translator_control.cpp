/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_translator.h"
#include "nucleus/cpu/util.h"
#include "nucleus/logger/logger.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"

#define INTERPRET(func) \
    builder.createCall(builder.getExternFunction( \
        reinterpret_cast<void*>( \
        reinterpret_cast<uintptr_t>( \
        static_cast<void(*)(Instruction)>([](Instruction o) { \
            auto& thread = *static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread()); \
            auto& state = *thread.state.get(); \
            func \
        }))), \
    TYPE_VOID, { TYPE_I32 }), { builder.getConstantI32(code.value) });

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */

void Translator::mfocrf(Instruction code)
{
    Value* cr = getCR();
    setGPR(code.rd, cr);
}

void Translator::mfspr(Instruction code)
{
    Value* rd;

    const U32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: // XER register
        rd = getXER();
        break;
    case 0x008: // LR register
        rd = getLR();
        break;
    case 0x009: // CTR register
        rd = getCTR();
        break;

    default:
        rd = builder.getConstantI64(0);
        logger.error(LOG_CPU, "Translator::mfspr error: Unknown SPR");
    }

    setGPR(code.rd, rd);
}

void Translator::mtocrf(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);

    if (code.l11) {
        int field = 0;
        int count = 0;
        for (int i = 0; i < 8; i++) {
            if (code.crm & (1 << i)) {
                field = (7 - i);
                count += 1;
            }
        }
        if (count == 1) {
            Value* value = builder.createTrunc(builder.createShr(rs, 4 * (7 - field)), TYPE_I8);
            value = builder.createAnd(value, builder.getConstantI8(0xF));
            setCRField(field, value);
        }
    } else {
        for (int field = 0; field < 8; field++) {
            if (code.crm & (1 << (7 - field))) {
                Value* value = builder.createTrunc(builder.createShr(rs, 4 * (7 - field)), TYPE_I8);
                value = builder.createAnd(value, builder.getConstantI8(0xF));
                setCRField(field, value);
            }
        }
    }
}

void Translator::mtspr(Instruction code)
{
    Value* rs = getGPR(code.rs);

    const U32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: // XER register
        setXER(rs);
        break;
    case 0x008: // LR register
        setLR(rs);
        break;
    case 0x009: // CTR register
        setCTR(rs);
        break;

    default:
        logger.error(LOG_CPU, "Translator::mtspr error: Unknown SPR");
    }
}

void Translator::mftb(Instruction code)
{
    hir::Function* timeFunc = builder.getExternFunction(reinterpret_cast<void*>(nucleusTime));
    hir::Value* timestamp = builder.createCall(timeFunc, {}, CALL_EXTERN);

    const U32 tbr = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (tbr) {
    case 0x10C:
        setGPR(code.rd, timestamp);
        break;

    case 0x10D:
        setGPR(code.rd, builder.createShl(timestamp, 32));
        break;

    default:
        assert_always("Invalid timebase register");
    }
}

void Translator::dcbf(Instruction code)
{
    // TODO
}

void Translator::dcbst(Instruction code)
{
    // TODO
}

void Translator::dcbt(Instruction code)
{
    // TODO
}

void Translator::dcbtst(Instruction code)
{
    // TODO
}

void Translator::dcbz(Instruction code)
{
    INTERPRET({
        const U32 addr = o.ra ? state.r[o.ra] + state.r[o.rb] : state.r[o.rb];
        void* ptr = thread.parent->memory->ptr(addr & ~127);
        std::memset(ptr, 0, 128);
    });
}

void Translator::icbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::eciwx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ecowx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
