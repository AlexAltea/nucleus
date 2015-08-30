/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

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
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory<I8>(addr);
    rd = builder.CreateZExt<I64>(result);
    setGPR(code.rd, rd);
}

void Recompiler::lbzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory<I8>(addr);
    rd = builder.CreateZExt<I64>(result);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lbzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I8>(addr);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lbzx(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I8>(addr);
    rd = builder.CreateZExt<I64>(result);

    setGPR(code.rd, rd);
}

void Recompiler::ld(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    rd = readMemory<I64>(addr);

    setGPR(code.rd, rd);
}

void Recompiler::ldarx(Instruction code)
{
}

void Recompiler::ldbrx(Instruction code)
{
}

void Recompiler::ldu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    rd = readMemory<I64>(addr);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::ldux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    rd = readMemory<I64>(addr);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::ldx(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    rd = readMemory<I64>(addr);

    setGPR(code.rd, rd);
}

void Recompiler::lfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    frd = builder.CreateBitCast<F64>(readMemory<I64>(addr));
    setFPR(code.frd, frd);
}

void Recompiler::lfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    frd = builder.CreateBitCast<F64>(readMemory<I64>(addr));
    addr = builder.createAdd(addr, getGPR(code.ra));

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Recompiler::lfdux(Instruction code)
{
    Value* addr;
    Value* frd;

    frd = builder.CreateBitCast<F64>(readMemory<I64>(addr));
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Recompiler::lfdx(Instruction code)
{
    Value* addr;
    Value* frd;

    frd = builder.CreateBitCast<F64>(readMemory<I64>(addr));
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));

    setFPR(code.frd, frd);
}

void Recompiler::lfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = builder.CreateBitCast<F32>(readMemory<I32>(addr));
    setFPR(code.frd, result);
}

void Recompiler::lfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    result = builder.CreateBitCast<F32>(readMemory<I32>(addr));
    addr = builder.createAdd(addr, getGPR(code.ra));

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Recompiler::lfsux(Instruction code)
{
    Value* addr;
    Value* result;

    result = builder.CreateBitCast<F32>(readMemory<I32>(addr));
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Recompiler::lfsx(Instruction code)
{
    Value* addr;
    Value* result;

    result = builder.CreateBitCast<F32>(readMemory<I32>(addr));
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));

    setFPR(code.frd, result);
}

void Recompiler::lha(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory<I16>(addr);
    rd = builder.CreateSExt<I64>(result);
    setGPR(code.rd, rd);
}

void Recompiler::lhau(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory<I16>(addr);
    rd = builder.CreateSExt<I64>(result);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhaux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I16>(addr);
    rd = builder.CreateSExt<I64>(result);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhax(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I16>(addr);
    rd = builder.CreateSExt<I64>(result);

    setGPR(code.rd, rd);
}

void Recompiler::lhbrx(Instruction code)
{
}

void Recompiler::lhz(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory<I16>(addr);
    rd = builder.CreateZExt<I64>(result);
    setGPR(code.rd, rd);
}

void Recompiler::lhzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory<I16>(addr);
    rd = builder.CreateZExt<I64>(result);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I16>(addr);
    rd = builder.CreateZExt<I64>(result);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lhzx(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory<I16>(addr);
    rd = builder.CreateZExt<I64>(result);

    setGPR(code.rd, rd);
}

void Recompiler::lmw(Instruction code)
{
}

void Recompiler::lswi(Instruction code)
{
}

void Recompiler::lswx(Instruction code)
{
}

void Recompiler::lwa(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateSExt<I64>(rd_i32);
    setGPR(code.rd, rd);
}

void Recompiler::lwarx(Instruction code)
{
}

void Recompiler::lwaux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateSExt<I64>(rd_i32);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwax(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateSExt<I64>(rd_i32);

    setGPR(code.rd, rd);
}

void Recompiler::lwbrx(Instruction code)
{
}

void Recompiler::lwz(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateZExt<I64>(rd_i32);
    setGPR(code.rd, rd);
}

void Recompiler::lwzu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateZExt<I64>(rd_i32);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwzux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateZExt<I64>(rd_i32);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Recompiler::lwzx(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory<I32>(addr);
    rd = builder.CreateZExt<I64>(rd_i32);

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
    Value* addr = builder.getConstantI64(code.ds << 2);
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
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I8);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
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
    Value* addr;
    Value* rs = getGPR(code.rs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);
}

void Recompiler::stfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto frs_i64 = builder.createBitCast<I64>(frs);
    writeMemory(addr, frs_i64);
}

void Recompiler::stfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    auto frs_i64 = builder.CreateBitCast<I64>(frs);
    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, frs_i64);

    setGPR(code.ra, addr);
}

void Recompiler::stfdux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    auto frs_i64 = builder.CreateBitCast<I64>(frs);
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs_i64);

    setGPR(code.ra, addr);
}

void Recompiler::stfdx(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    auto frs_i64 = builder.CreateBitCast<I64>(frs);
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs_i64);
}

void Recompiler::stfiwx(Instruction code)
{
}

void Recompiler::stfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto frs_f32 = builder.CreateFPTrunc<F32>(frs);
    auto frs_i32 = builder.CreateBitCast<I32>(frs_f32);
    writeMemory(addr, frs_i32);
}

void Recompiler::stfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    auto frs_f32 = builder.CreateFPTrunc<F32>(frs);
    auto frs_i32 = builder.CreateBitCast<I32>(frs_f32);
    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, frs_i32);

    setGPR(code.ra, addr);
}

void Recompiler::stfsux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    auto frs_f32 = builder.CreateFPTrunc<F32>(frs);
    auto frs_i32 = builder.CreateBitCast<I32>(frs_f32);
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs_i32);

    setGPR(code.ra, addr);
}

void Recompiler::stfsx(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    auto frs_f32 = builder.CreateFPTrunc<F32>(frs);
    auto frs_i32 = builder.CreateBitCast<I32>(frs_f32);
    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs_i32);
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
}

void Recompiler::sthu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
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
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I16);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);
}

void Recompiler::stmw(Instruction code)
{
}

void Recompiler::stswi(Instruction code)
{
}

void Recompiler::stswx(Instruction code)
{
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
}

void Recompiler::stwcx_(Instruction code)
{
}

void Recompiler::stwu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
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
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I32);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);
}

void Recompiler::eieio(Instruction code)
{
}

void Recompiler::sync(Instruction code)
{
}

void Recompiler::isync(Instruction code)
{
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
