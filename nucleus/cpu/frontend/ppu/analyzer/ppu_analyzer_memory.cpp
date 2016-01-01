/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace frontend {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Load and Store Instructions (Section: 4.2.3)
 *  - UISA: Memory Synchronization Instructions (Section: 4.2.6)
 *  - VEA: Memory Synchronization Instructions (Section: 4.3.2)
 */

void Analyzer::lbz(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lbzu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lbzux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lbzx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::ld(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::ldarx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    //thread.reserve_addr = addr;
    //thread.reserve_value = re64(thread.gpr[code.rd]);
}

void Analyzer::ldbrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::ldu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::ldux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::ldx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lfd(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
}

void Analyzer::lfdu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lfdux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lfdx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
}

void Analyzer::lfs(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
}

void Analyzer::lfsu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lfsux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lfsx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
}

void Analyzer::lha(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lhau(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lhaux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lhax(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lhbrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lhz(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lhzu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lhzux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lhzx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lmw(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    for (U32 i = code.rd; i < 32; i++) {
        setFlag(gpr[i], REG_WRITE);
    }
}

void Analyzer::lswi(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    U8 reg = code.rd;
    U64 n = code.nb ? code.nb : 32;
    while (n > 0) {
        setFlag(gpr[reg], REG_WRITE);
        n = (n > 3) ? (n - 4) : 0;
        reg = (reg + 1) % 32;
    }
}

void Analyzer::lswx(Instruction code)
{
    // TODO: ?
}

void Analyzer::lwa(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lwarx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    //thread.reserve_addr = addr;
    //thread.reserve_value = re32(thread.gpr[code.rd]);
}

void Analyzer::lwaux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lwax(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lwbrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lwz(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::lwzu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lwzux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::lwzx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::stb(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stbu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stbux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stbx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::std(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stdcx_(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    //thread.reserve_addr REG_READ
    //thread.reserve_value REG_READ
    setFlag(cr[0], REG_WRITE);
}

void Analyzer::stdu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stdux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stdx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stfd(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
}

void Analyzer::stfdu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stfdux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stfdx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
}

void Analyzer::stfiwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
}

void Analyzer::stfs(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
}

void Analyzer::stfsu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stfsux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stfsx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(fpr[code.frs], REG_READ);
}

void Analyzer::sth(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::sthbrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::sthu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::sthux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::sthx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stmw(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    for (int i = code.rs; i < 32; i++) {
        setFlag(gpr[i], REG_READ);
    }
}

void Analyzer::stswi(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    U64 n = code.nb ? code.nb : 32;
    U8 reg = code.rd;
    while (n > 0) {
        setFlag(gpr[reg], REG_READ);
        n = (n > 3) ? (n - 4) : 0;
        reg = (reg + 1) % 32;
    }
}

void Analyzer::stswx(Instruction code)
{
    // TODO: ?
}

void Analyzer::stw(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stwbrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::stwcx_(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    //thread.reserve_addr REG_READ
    //thread.reserve_value REG_READ
    setFlag(cr[0], REG_WRITE);
}

void Analyzer::stwu(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stwux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::stwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

void Analyzer::eieio(Instruction code)
{
    // TODO: ?
}

void Analyzer::sync(Instruction code)
{
    // TODO: ?
}

void Analyzer::isync(Instruction code)
{
    // TODO: ?
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
