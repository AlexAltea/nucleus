/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Load and Store Instructions (Section: 4.2.3)
 *  - UISA: Memory Synchronization Instructions (Section: 4.2.6)
 *  - VEA: Memory Synchronization Instructions (Section: 4.3.2)
 */

void Recompiler::lbz(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::lbzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lbzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lbzx(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::ld(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::ldarx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    rd = readMemory(addr, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::ldbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ldu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::ldux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::ldx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::lfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    frd = readMemory(addr, TYPE_F64);
    setFPR(code.frd, frd);
}

void Recompiler::lfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    frd = readMemory(addr, TYPE_F64);

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Recompiler::lfdux(Instruction code)
{
    Value* addr;
    Value* frd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    frd = readMemory(addr, TYPE_F64);

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Recompiler::lfdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    frd = readMemory(addr, TYPE_F64);

    setFPR(code.frd, frd);
}

void Recompiler::lfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_F32);
    setFPR(code.frd, result);
}

void Recompiler::lfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_F32);

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Recompiler::lfsux(Instruction code)
{
    Value* addr;
    Value* result;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_F32);

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Recompiler::lfsx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* result;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_F32);

    setFPR(code.frd, result);
}

void Recompiler::lha(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::lhau(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhaux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhax(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::lhbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::lhz(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::lhzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhzx(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::lmw(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::lswi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::lswx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::lwa(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::lwarx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    rd = readMemory(addr, TYPE_I32);
    setGPR(code.rd, rd);
}

void Recompiler::lwaux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwax(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::lwbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::lwz(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);
    setGPR(code.rd, rd);
}

void Recompiler::lwzu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwzux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwzx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.rd, rd);
}

void Recompiler::stb(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I8);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Recompiler::stbu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I8);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stbux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I8);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stbx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I8);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Recompiler::std(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Recompiler::stdcx_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    // TODO: Is this correct?
    setCRField(0, builder.getConstantI8(2));

    writeMemory(addr, rs);
}

void Recompiler::stdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rs = getGPR(code.rs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stdux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Recompiler::stfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, frs);
}

void Recompiler::stfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, frs);

    setGPR(code.ra, addr);
}

void Recompiler::stfdux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs);

    setGPR(code.ra, addr);
}

void Recompiler::stfdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, frs);
}

void Recompiler::stfiwx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::stfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);
}

void Recompiler::stfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);

    setGPR(code.ra, addr);
}

void Recompiler::stfsux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);

    setGPR(code.ra, addr);
}

void Recompiler::stfsx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);
}

void Recompiler::sth(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I16);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Recompiler::sthbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sthu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I16);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::sthux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I16);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::sthx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I16);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Recompiler::stmw(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::stswi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::stswx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::stw(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I32);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Recompiler::stwbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::stwcx_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I32);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    // TODO: Is this correct?
    setCRField(0, builder.getConstantI8(2));

    writeMemory(addr, rs);
}

void Recompiler::stwu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I32);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stwux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I32);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Recompiler::stwx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I32);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Recompiler::eieio(Instruction code)
{
    builder.createMemFence();
}

void Recompiler::sync(Instruction code)
{
    builder.createMemFence();
}

void Recompiler::isync(Instruction code)
{
    // TODO
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
