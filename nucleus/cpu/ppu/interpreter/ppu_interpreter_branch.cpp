/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"

namespace cpu {
namespace ppu {

bool CheckCondition(PPUThread& thread, u32 bo, u32 bi)
{
    const u8 bo0 = (bo & 0x10) ? 1 : 0;
    const u8 bo1 = (bo & 0x08) ? 1 : 0;
    const u8 bo2 = (bo & 0x04) ? 1 : 0;
    const u8 bo3 = (bo & 0x02) ? 1 : 0;

    if (!bo2) --thread.ctr;

    const u8 ctr_ok = bo2 | ((thread.ctr != 0) ^ bo3);
    const u8 cond_ok = bo0 | (thread.cr.getBit(bi) ^ (~bo1 & 0x1));

    return ctr_ok && cond_ok;
}

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void Interpreter::bx(Instruction code, PPUThread& thread)
{
    if (code.lk) thread.lr = thread.pc + 4;
    thread.pc = (code.aa ? (code.li << 2) : thread.pc + (code.li << 2)) & ~0x3ULL;
    thread.pc -= 4;
}

void Interpreter::bcx(Instruction code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = (code.aa ? (code.bd << 2) : thread.pc + (code.bd << 2)) & ~0x3ULL;
        thread.pc -= 4;
    }
}

void Interpreter::bcctrx(Instruction code, PPUThread& thread)
{
    if (code.bo & 0x10 || thread.cr.getBit(code.bi) == ((code.bo >> 3) & 1)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = thread.ctr & ~0x3ULL;
        thread.pc -= 4;
    }
}

void Interpreter::bclrx(Instruction code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        const u32 newLR = thread.pc + 4;
        thread.pc = thread.lr & ~0x3ULL;
        thread.pc -= 4;
        if (code.lk) thread.lr = newLR;
    }
}

void Interpreter::crand(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::crandc(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::creqv(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::crnand(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::crnor(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::cror(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::crorc(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::crxor(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}

void Interpreter::mcrf(Instruction code, PPUThread& thread)
{
    thread.cr.setField(code.crfd, thread.cr.getField(code.crfs));
}

void Interpreter::sc(Instruction code, PPUThread& thread)
{
    switch (code.lev) {
    case 0:
        nucleus.lv2.call(thread);
        break;
    case 1:
        unknown("hvsc");
        break;
    case 2:
        nucleus.lv2.modules.call(thread);
        break;
    default:
        unknown("sc");
    }
}

void Interpreter::td(Instruction code, PPUThread& thread)
{
    unknown("td");
}

void Interpreter::tdi(Instruction code, PPUThread& thread)
{
    unknown("tdi");
}

void Interpreter::tw(Instruction code, PPUThread& thread)
{
    unknown("tw");
}

void Interpreter::twi(Instruction code, PPUThread& thread)
{
    unknown("twi");
}

}  // namespace ppu
}  // namespace cpu
