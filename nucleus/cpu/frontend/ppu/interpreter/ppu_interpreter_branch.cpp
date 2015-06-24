/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"

namespace cpu {
namespace ppu {

bool CheckCondition(State& state, u32 bo, u32 bi)
{
    const u8 bo0 = (bo & 0x10) ? 1 : 0;
    const u8 bo1 = (bo & 0x08) ? 1 : 0;
    const u8 bo2 = (bo & 0x04) ? 1 : 0;
    const u8 bo3 = (bo & 0x02) ? 1 : 0;

    if (!bo2) {
        state.ctr--;
    }

    const u8 ctr_ok = bo2 | ((state.ctr != 0) ^ bo3);
    const u8 cond_ok = bo0 | (state.cr.getBit(bi) ^ (~bo1 & 0x1));
    return ctr_ok && cond_ok;
}

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void Interpreter::bx(Instruction code)
{
    if (code.lk) state.lr = state.pc + 4;
    state.pc = (code.aa ? (code.li << 2) : state.pc + (code.li << 2)) & ~0x3ULL;
    state.pc -= 4;
}

void Interpreter::bcx(Instruction code)
{
    if (CheckCondition(state, code.bo, code.bi)) {
        if (code.lk) state.lr = state.pc + 4;
        state.pc = (code.aa ? (code.bd << 2) : state.pc + (code.bd << 2)) & ~0x3ULL;
        state.pc -= 4;
    }
}

void Interpreter::bcctrx(Instruction code)
{
    if (code.bo & 0x10 || state.cr.getBit(code.bi) == ((code.bo >> 3) & 1)) {
        if (code.lk) state.lr = state.pc + 4;
        state.pc = state.ctr & ~0x3ULL;
        state.pc -= 4;
    }
}

void Interpreter::bclrx(Instruction code)
{
    if (CheckCondition(state, code.bo, code.bi)) {
        const u32 newLR = state.pc + 4;
        state.pc = state.lr & ~0x3ULL;
        state.pc -= 4;
        if (code.lk) state.lr = newLR;
    }
}

void Interpreter::crand(Instruction code)
{
    const u8 value = state.cr.getBit(code.crba) & state.cr.getBit(code.crbb);
    state.cr.setBit(code.crbd, value);
}

void Interpreter::crandc(Instruction code)
{
    const u8 value = state.cr.getBit(code.crba) & (1 ^ state.cr.getBit(code.crbb));
    state.cr.setBit(code.crbd, value);
}

void Interpreter::creqv(Instruction code)
{
    const u8 value = 1 ^ (state.cr.getBit(code.crba) ^ state.cr.getBit(code.crbb));
    state.cr.setBit(code.crbd, value);
}

void Interpreter::crnand(Instruction code)
{
    const u8 value = 1 ^ (state.cr.getBit(code.crba) & state.cr.getBit(code.crbb));
    state.cr.setBit(code.crbd, value);
}

void Interpreter::crnor(Instruction code)
{
    const u8 value = 1 ^ (state.cr.getBit(code.crba) | state.cr.getBit(code.crbb));
    state.cr.setBit(code.crbd, value);
}

void Interpreter::cror(Instruction code)
{
    const u8 value = state.cr.getBit(code.crba) | state.cr.getBit(code.crbb);
    state.cr.setBit(code.crbd, value);
}

void Interpreter::crorc(Instruction code)
{
    const u8 value = state.cr.getBit(code.crba) | (1 ^ state.cr.getBit(code.crbb));
    state.cr.setBit(code.crbd, value);
}

void Interpreter::crxor(Instruction code)
{
    const u8 value = state.cr.getBit(code.crba) ^ state.cr.getBit(code.crbb);
    state.cr.setBit(code.crbd, value);
}

void Interpreter::mcrf(Instruction code)
{
    state.cr.setField(code.crfd, state.cr.getField(code.crfs));
}

void Interpreter::sc(Instruction code)
{
    switch (code.lev) {
    case 0:
        nucleus.lv2.call(state);
        break;
    case 1:
        unknown("hvsc");
        break;
    case 2:
        nucleus.lv2.modules.call(state);
        break;
    default:
        unknown("sc");
    }
}

void Interpreter::td(Instruction code)
{
    unknown("td");
}

void Interpreter::tdi(Instruction code)
{
    unknown("tdi");
}

void Interpreter::tw(Instruction code)
{
    unknown("tw");
}

void Interpreter::twi(Instruction code)
{
    unknown("twi");
}

}  // namespace ppu
}  // namespace cpu
