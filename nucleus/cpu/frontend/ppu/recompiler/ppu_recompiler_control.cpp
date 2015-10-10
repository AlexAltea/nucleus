/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/cpu/util.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/assert.h"

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

void Recompiler::mfocrf(Instruction code)
{
    Value* cr = getCR();
    setGPR(code.rd, cr);
}

void Recompiler::mfspr(Instruction code)
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
        logger.error(LOG_CPU, "Recompiler::mfspr error: Unknown SPR");
    }

    setGPR(code.rd, rd);
}

void Recompiler::mtocrf(Instruction code)
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
            Value* value = builder.createShr(rs, (7 - field) * 4);
            value = builder.createAnd(value, builder.getConstantI32(0xF));
            setCR(field, value);
        }
    } else {
        setCR(rs);
    }
}

void Recompiler::mtspr(Instruction code)
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
        logger.error(LOG_CPU, "Recompiler::mtspr error: Unknown SPR");
    }
}

void Recompiler::mftb(Instruction code)
{
    hir::Function* timeFunc = builder.getExternFunction(nucleusTime);
    hir::Value* timestamp = builder.createCall(timeFunc, {}, CALL_EXTERN);

    const U32 tbr = (code.spr >> 5) | ((code.spr & 0x1f) << 5);
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

void Recompiler::dcbf(Instruction code)
{
    // TODO
}

void Recompiler::dcbst(Instruction code)
{
    // TODO
}

void Recompiler::dcbt(Instruction code)
{
    // TODO
}

void Recompiler::dcbtst(Instruction code)
{
    // TODO
}

void Recompiler::dcbz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::icbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::eciwx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ecowx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
