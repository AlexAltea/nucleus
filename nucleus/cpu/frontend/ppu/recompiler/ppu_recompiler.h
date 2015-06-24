/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/builder.h"
#include "nucleus/cpu/hir/value.h"
#include "nucleus/cpu/frontend/frontend_recompiler.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/cpu/frontend/ppu/ppu_instruction.h"

#include "llvm/IR/Intrinsics.h"

namespace cpu {
namespace ppu {

class Recompiler : public frontend::IRecompiler<u32>
{
    // Register allocation
    hir::Value<hir::I64*> gpr[32];
    hir::Value<hir::F64*> fpr[32];
    hir::Value<hir::I128*> vr[32];
    hir::Value<hir::I8*> cr[8];
    hir::Value<hir::I32*> fpscr;
    hir::Value<hir::I64*> xer;
    hir::Value<hir::I64*> ctr;

    template <typename T>
    hir::Value<T*> allocaVariable(const std::string& name) {
        hir::Block entryBlock = function->function.getEntryBlock();
        hir::Builder allocaBuilder;
        allocaBuilder.SetInsertPoint(entryBlock, entryBlock.begin());
        return allocaBuilder.CreateAlloca<T>(name);
    }

    /**
     * Register read
     */
    hir::Value<hir::I64> getGPR(int index);
    hir::Value<hir::F64> getFPR(int index);
    hir::Value<hir::I8> getCR(int index);
    hir::Value<hir::I64> getXER();
    hir::Value<hir::I64> getCTR();

    template <typename T=I64>
    hir::Value<T> getGPR(int index) {
        static_assert(std::is_integral<T::type>::value,
            "ppu::Recompiler::getGPR accepts only integer values");
        static_assert(T::size < 64,
            "ppu::Recompiler::getGPR accepts only up to 64-bit integer values");
        return builder.CreateTrunc<T>(getGPR(index));
    }

    template <typename T>
    hir::Value<T, 128 / T::size> getVR(int index) {
        // TODO: ?
        return hir::Value<T, 128 / T::size> {};
    }

    /**
     * Register write
     */
    void setGPR(int index, hir::Value<hir::I64> value);
    void setFPR(int index, hir::Value<hir::F64> value);
    void setCR(int index, hir::Value<hir::I8> value);
    void setXER(hir::Value<hir::I64> value);
    void setCTR(hir::Value<hir::I64> value);

    void setFPR(int index, hir::Value<hir::I32> value) {
        setFPR(index, builder.CreateFPExt<hir::F64>(builder.CreateBitCast<hir::F32>(value)));
    }
    void setFPR(int index, hir::Value<hir::I64> value) {
        setFPR(index, builder.CreateBitCast<hir::F64>(value));
    }
    void setFPR(int index, hir::Value<hir::F32> value) {
        setFPR(index, builder.CreateFPExt<hir::F64>(value));
    }

    template <typename T, int N>
    void setVR(int index, hir::Value<T, N> value) {
        static_assert(T::size * N == 128,
            "ppu::Recompiler::getGPR accepts only 32-bit or 64-bit arithmetic values");

        if (!vr[index]) {
            vr[index] = allocaVariable<hir::I128>("vrTEST");
        }

        auto value_i128 = builder.CreateBitCast<hir::I128>(value);
        builder.CreateStore(value_i128, vr[index]);
    }

    // State pointer allocation
    hir::Value<StateType*> state;

    /**
     * Operation flags
     */
    void updateCR0(hir::Value<hir::I64> value); // Integer instructions with RC bit
    void updateCR1(llvm::Value* value); // Floating-Point instructions with RC bit
    void updateCR6(llvm::Value* value); // Vector instructions with RC bit

    template <typename T>
    void updateCR(int field, hir::Value<T> lhs, hir::Value<T> rhs, bool logicalComparison) {
        hir::Value<I1> isLT;
        hir::Value<I1> isGT;
        hir::Value<I8> cr;

        if (logicalComparison) {
            isLT = builder.CreateICmpULT(lhs, rhs);
            isGT = builder.CreateICmpUGT(lhs, rhs);
        } else {
            isLT = builder.CreateICmpSLT(lhs, rhs);
            isGT = builder.CreateICmpSGT(lhs, rhs);
        }

        cr = builder.CreateSelect(isGT, builder.get<hir::I8>(2), builder.get<hir::I8>(4));
        cr = builder.CreateSelect(isLT, builder.get<hir::I8>(1), cr);
        setCR(field, cr);
    }

    /**
     * Memory access
     */
    // Read specified number of bits from memory swapping endianness if necessary
    template <typename T>
    hir::Value<T> readMemory(hir::Value<hir::I64> addr) {
        auto ppuSegment = static_cast<Segment*>(function->parent);
        hir::Value<hir::I64> baseAddr = builder.CreateLoad(ppuSegment->memoryBase);

        addr = builder.CreateAdd(addr, baseAddr);
        auto pointer = builder.CreateIntToPtr<T>(addr);
        auto value = builder.CreateLoad(pointer);
        return builder.CreateIntrinsic_Bswap(value);
    }

    template <>
    hir::Value<hir::I8> readMemory(hir::Value<hir::I64> addr) {
        auto ppuSegment = static_cast<Segment*>(function->parent);
        hir::Value<hir::I64> baseAddr = builder.CreateLoad(ppuSegment->memoryBase);

        addr = builder.CreateAdd(addr, baseAddr);
        auto pointer = builder.CreateIntToPtr<hir::I8>(addr);
        auto value = builder.CreateLoad(pointer);
        return value;
    }

    // Write value to memory swapping endianness if necessary
    template <typename T>
    void writeMemory(hir::Value<hir::I64> addr, hir::Value<T> value) {
        auto ppuSegment = static_cast<Segment*>(function->parent);
        hir::Value<hir::I64> baseAddr = builder.CreateLoad(ppuSegment->memoryBase);

        value = builder.CreateIntrinsic_Bswap(value);
        addr = builder.CreateAdd(addr, baseAddr);
        auto pointer = builder.CreateIntToPtr<T>(addr);
        builder.CreateStore(value, pointer);
    }

    void writeMemory(hir::Value<hir::I64> addr, hir::Value<hir::I8> value) {
        auto ppuSegment = static_cast<Segment*>(function->parent);
        hir::Value<hir::I64> baseAddr = builder.CreateLoad(ppuSegment->memoryBase);

        addr = builder.CreateAdd(addr, baseAddr);
        auto pointer = builder.CreateIntToPtr<hir::I8>(addr);
        builder.CreateStore(value, pointer);
    }

public:
    hir::Builder builder;

    Recompiler(Function* function);

    void createProlog();
    void createEpilog();

    // Recompiler status
    u32 currentAddress;

    /**
     * PPC64 Instructions:
     * Organized according to the chapter 4 of the Programming Environments Manual
     * for 64-bit PowerPC Microprocessors (Version 3.0 / July 15, 2005).
     */

    // UISA: Integer instructions (Section: 4.2.1)
    void addx(Instruction code);
    void addcx(Instruction code);
    void addex(Instruction code);
    void addi(Instruction code);
    void addic(Instruction code);
    void addic_(Instruction code);
    void addis(Instruction code);
    void addmex(Instruction code);
    void addzex(Instruction code);
    void andx(Instruction code);
    void andcx(Instruction code);
    void andi_(Instruction code);
    void andis_(Instruction code);
    void cmp(Instruction code);
    void cmpi(Instruction code);
    void cmpl(Instruction code);
    void cmpli(Instruction code);
    void cntlzdx(Instruction code);
    void cntlzwx(Instruction code);
    void divdx(Instruction code);
    void divdux(Instruction code);
    void divwx(Instruction code);
    void divwux(Instruction code);
    void eqvx(Instruction code);
    void extsbx(Instruction code);
    void extshx(Instruction code);
    void extswx(Instruction code);
    void mulhdx(Instruction code);
    void mulhdux(Instruction code);
    void mulhwx(Instruction code);
    void mulhwux(Instruction code);
    void mulldx(Instruction code);
    void mulli(Instruction code);
    void mullwx(Instruction code);
    void nandx(Instruction code);
    void negx(Instruction code);
    void norx(Instruction code);
    void orx(Instruction code);
    void orcx(Instruction code);
    void ori(Instruction code);
    void oris(Instruction code);
    void rldc_lr(Instruction code);
    void rldicx(Instruction code);
    void rldiclx(Instruction code);
    void rldicrx(Instruction code);
    void rldimix(Instruction code);
    void rlwimix(Instruction code);
    void rlwinmx(Instruction code);
    void rlwnmx(Instruction code);
    void sldx(Instruction code);
    void slwx(Instruction code);
    void sradx(Instruction code);
    void sradix(Instruction code);
    void srawx(Instruction code);
    void srawix(Instruction code);
    void srdx(Instruction code);
    void srwx(Instruction code);
    void subfx(Instruction code);
    void subfcx(Instruction code);
    void subfex(Instruction code);
    void subfic(Instruction code);
    void subfmex(Instruction code);
    void subfzex(Instruction code);
    void xorx(Instruction code);
    void xori(Instruction code);
    void xoris(Instruction code);

    // UISA: Floating-Point Instructions (Section: 4.2.2)
    void fabsx(Instruction code);
    void faddx(Instruction code);
    void faddsx(Instruction code);
    void fcfidx(Instruction code);
    void fcmpo(Instruction code);
    void fcmpu(Instruction code);
    void fctidx(Instruction code);
    void fctidzx(Instruction code);
    void fctiwx(Instruction code);
    void fctiwzx(Instruction code);
    void fdivx(Instruction code);
    void fdivsx(Instruction code);
    void fmaddx(Instruction code);
    void fmaddsx(Instruction code);
    void fmrx(Instruction code);
    void fmsubx(Instruction code);
    void fmsubsx(Instruction code);
    void fmulx(Instruction code);
    void fmulsx(Instruction code);
    void fnabsx(Instruction code);
    void fnegx(Instruction code);
    void fnmaddx(Instruction code);
    void fnmaddsx(Instruction code);
    void fnmsubx(Instruction code);
    void fnmsubsx(Instruction code);
    void fresx(Instruction code);
    void frspx(Instruction code);
    void frsqrtex(Instruction code);
    void fselx(Instruction code);
    void fsqrtx(Instruction code);
    void fsqrtsx(Instruction code);
    void fsubx(Instruction code);
    void fsubsx(Instruction code);
    void mcrfs(Instruction code);
    void mffsx(Instruction code);
    void mtfsb0x(Instruction code);
    void mtfsb1x(Instruction code);
    void mtfsfix(Instruction code);
    void mtfsfx(Instruction code);

    // UISA: Load and Store Instructions (Section: 4.2.3)
    void lbz(Instruction code);
    void lbzu(Instruction code);
    void lbzux(Instruction code);
    void lbzx(Instruction code);
    void ld(Instruction code);
    void ldbrx(Instruction code);
    void ldu(Instruction code);
    void ldux(Instruction code);
    void ldx(Instruction code);
    void lfd(Instruction code);
    void lfdu(Instruction code);
    void lfdux(Instruction code);
    void lfdx(Instruction code);
    void lfs(Instruction code);
    void lfsu(Instruction code);
    void lfsux(Instruction code);
    void lfsx(Instruction code);
    void lha(Instruction code);
    void lhau(Instruction code);
    void lhaux(Instruction code);
    void lhax(Instruction code);
    void lhbrx(Instruction code);
    void lhz(Instruction code);
    void lhzu(Instruction code);
    void lhzux(Instruction code);
    void lhzx(Instruction code);
    void lmw(Instruction code);
    void lswi(Instruction code);
    void lswx(Instruction code);
    void lwa(Instruction code);
    void lwaux(Instruction code);
    void lwax(Instruction code);
    void lwbrx(Instruction code);
    void lwz(Instruction code);
    void lwzu(Instruction code);
    void lwzux(Instruction code);
    void lwzx(Instruction code);
    void stb(Instruction code);
    void stbu(Instruction code);
    void stbux(Instruction code);
    void stbx(Instruction code);
    void std(Instruction code);
    void stdu(Instruction code);
    void stdux(Instruction code);
    void stdx(Instruction code);
    void stfd(Instruction code);
    void stfdu(Instruction code);
    void stfdux(Instruction code);
    void stfdx(Instruction code);
    void stfiwx(Instruction code);
    void stfs(Instruction code);
    void stfsu(Instruction code);
    void stfsux(Instruction code);
    void stfsx(Instruction code);
    void sth(Instruction code);
    void sthbrx(Instruction code);
    void sthu(Instruction code);
    void sthux(Instruction code);
    void sthx(Instruction code);
    void stmw(Instruction code);
    void stswi(Instruction code);
    void stswx(Instruction code);
    void stw(Instruction code);
    void stwbrx(Instruction code);
    void stwu(Instruction code);
    void stwux(Instruction code);
    void stwx(Instruction code);

    // UISA: Branch and Flow Control Instructions (Section: 4.2.4)
    void bx(Instruction code);
    void bcx(Instruction code);
    void bcctrx(Instruction code);
    void bclrx(Instruction code);
    void crand(Instruction code);
    void crandc(Instruction code);
    void creqv(Instruction code);
    void crnand(Instruction code);
    void crnor(Instruction code);
    void cror(Instruction code);
    void crorc(Instruction code);
    void crxor(Instruction code);
    void mcrf(Instruction code);
    void sc(Instruction code);
    void td(Instruction code);
    void tdi(Instruction code);
    void tw(Instruction code);
    void twi(Instruction code);

    // UISA: Processor Control Instructions (Section: 4.2.5)
    void mfocrf(Instruction code);
    void mfspr(Instruction code);
    void mtocrf(Instruction code);
    void mtspr(Instruction code);

    // UISA: Memory Synchronization Instructions (Section: 4.2.6)
    void ldarx(Instruction code);
    void lwarx(Instruction code);
    void stdcx_(Instruction code);
    void stwcx_(Instruction code);
    void sync(Instruction code);

    // VEA: Processor Control Instructions (Section: 4.3.1)
    void mftb(Instruction code);

    // VEA: Memory Synchronization Instructions (Section: 4.3.2)
    void eieio(Instruction code);
    void isync(Instruction code);

    // VEA: Memory Control Instructions (Section: 4.3.3)
    void dcbf(Instruction code);
    void dcbst(Instruction code);
    void dcbt(Instruction code);
    void dcbtst(Instruction code);
    void dcbz(Instruction code);
    void icbi(Instruction code);

    // VEA: External Control Instructions (Section: 4.3.4)
    void eciwx(Instruction code);
    void ecowx(Instruction code);

    /**
     * PPC64 Vector/SIMD Instructions (aka AltiVec):
     * Organized according to the chapter 4 of the Programming Environments Manual of the Vector/SIMD
     * Multimedia Extension Technology for 64-bit PowerPC Microprocessors (Version 2.07c / October 26, 2006).
     */

    void dss(Instruction code);
    void dst(Instruction code);
    void dstst(Instruction code);
    void lvebx(Instruction code);
    void lvehx(Instruction code);
    void lvewx(Instruction code);
    void lvlx(Instruction code);
    void lvlxl(Instruction code);
    void lvrx(Instruction code);
    void lvrxl(Instruction code);
    void lvsl(Instruction code);
    void lvsr(Instruction code);
    void lvx(Instruction code);
    void lvxl(Instruction code);
    void mfvscr(Instruction code);
    void mtvscr(Instruction code);
    void stvebx(Instruction code);
    void stvehx(Instruction code);
    void stvewx(Instruction code);
    void stvlx(Instruction code);
    void stvlxl(Instruction code);
    void stvrx(Instruction code);
    void stvrxl(Instruction code);
    void stvx(Instruction code);
    void stvxl(Instruction code);
    void vaddcuw(Instruction code);
    void vaddfp(Instruction code);
    void vaddsbs(Instruction code);
    void vaddshs(Instruction code);
    void vaddsws(Instruction code);
    void vaddubm(Instruction code);
    void vaddubs(Instruction code);
    void vadduhm(Instruction code);
    void vadduhs(Instruction code);
    void vadduwm(Instruction code);
    void vadduws(Instruction code);
    void vand(Instruction code);
    void vandc(Instruction code);
    void vavgsb(Instruction code);
    void vavgsh(Instruction code);
    void vavgsw(Instruction code);
    void vavgub(Instruction code);
    void vavguh(Instruction code);
    void vavguw(Instruction code);
    void vcfsx(Instruction code);
    void vcfux(Instruction code);
    void vcmpbfp(Instruction code);
    void vcmpbfp_(Instruction code);
    void vcmpeqfp(Instruction code);
    void vcmpeqfp_(Instruction code);
    void vcmpequb(Instruction code);
    void vcmpequb_(Instruction code);
    void vcmpequh(Instruction code);
    void vcmpequh_(Instruction code);
    void vcmpequw(Instruction code);
    void vcmpequw_(Instruction code);
    void vcmpgefp(Instruction code);
    void vcmpgefp_(Instruction code);
    void vcmpgtfp(Instruction code);
    void vcmpgtfp_(Instruction code);
    void vcmpgtsb(Instruction code);
    void vcmpgtsb_(Instruction code);
    void vcmpgtsh(Instruction code);
    void vcmpgtsh_(Instruction code);
    void vcmpgtsw(Instruction code);
    void vcmpgtsw_(Instruction code);
    void vcmpgtub(Instruction code);
    void vcmpgtub_(Instruction code);
    void vcmpgtuh(Instruction code);
    void vcmpgtuh_(Instruction code);
    void vcmpgtuw(Instruction code);
    void vcmpgtuw_(Instruction code);
    void vctsxs(Instruction code);
    void vctuxs(Instruction code);
    void vexptefp(Instruction code);
    void vlogefp(Instruction code);
    void vmaddfp(Instruction code);
    void vmaxfp(Instruction code);
    void vmaxsb(Instruction code);
    void vmaxsh(Instruction code);
    void vmaxsw(Instruction code);
    void vmaxub(Instruction code);
    void vmaxuh(Instruction code);
    void vmaxuw(Instruction code);
    void vmhaddshs(Instruction code);
    void vmhraddshs(Instruction code);
    void vminfp(Instruction code);
    void vminsb(Instruction code);
    void vminsh(Instruction code);
    void vminsw(Instruction code);
    void vminub(Instruction code);
    void vminuh(Instruction code);
    void vminuw(Instruction code);
    void vmladduhm(Instruction code);
    void vmrghb(Instruction code);
    void vmrghh(Instruction code);
    void vmrghw(Instruction code);
    void vmrglb(Instruction code);
    void vmrglh(Instruction code);
    void vmrglw(Instruction code);
    void vmsummbm(Instruction code);
    void vmsumshm(Instruction code);
    void vmsumshs(Instruction code);
    void vmsumubm(Instruction code);
    void vmsumuhm(Instruction code);
    void vmsumuhs(Instruction code);
    void vmulesb(Instruction code);
    void vmulesh(Instruction code);
    void vmuleub(Instruction code);
    void vmuleuh(Instruction code);
    void vmulosb(Instruction code);
    void vmulosh(Instruction code);
    void vmuloub(Instruction code);
    void vmulouh(Instruction code);
    void vnmsubfp(Instruction code);
    void vnor(Instruction code);
    void vor(Instruction code);
    void vperm(Instruction code);
    void vpkpx(Instruction code);
    void vpkshss(Instruction code);
    void vpkshus(Instruction code);
    void vpkswss(Instruction code);
    void vpkswus(Instruction code);
    void vpkuhum(Instruction code);
    void vpkuhus(Instruction code);
    void vpkuwum(Instruction code);
    void vpkuwus(Instruction code);
    void vrefp(Instruction code);
    void vrfim(Instruction code);
    void vrfin(Instruction code);
    void vrfip(Instruction code);
    void vrfiz(Instruction code);
    void vrlb(Instruction code);
    void vrlh(Instruction code);
    void vrlw(Instruction code);
    void vrsqrtefp(Instruction code);
    void vsel(Instruction code);
    void vsl(Instruction code);
    void vslb(Instruction code);
    void vsldoi(Instruction code);
    void vslh(Instruction code);
    void vslo(Instruction code);
    void vslw(Instruction code);
    void vspltb(Instruction code);
    void vsplth(Instruction code);
    void vspltisb(Instruction code);
    void vspltish(Instruction code);
    void vspltisw(Instruction code);
    void vspltw(Instruction code);
    void vsr(Instruction code);
    void vsrab(Instruction code);
    void vsrah(Instruction code);
    void vsraw(Instruction code);
    void vsrb(Instruction code);
    void vsrh(Instruction code);
    void vsro(Instruction code);
    void vsrw(Instruction code);
    void vsubcuw(Instruction code);
    void vsubfp(Instruction code);
    void vsubsbs(Instruction code);
    void vsubshs(Instruction code);
    void vsubsws(Instruction code);
    void vsububm(Instruction code);
    void vsububs(Instruction code);
    void vsubuhm(Instruction code);
    void vsubuhs(Instruction code);
    void vsubuwm(Instruction code);
    void vsubuws(Instruction code);
    void vsum2sws(Instruction code);
    void vsum4sbs(Instruction code);
    void vsum4shs(Instruction code);
    void vsum4ubs(Instruction code);
    void vsumsws(Instruction code);
    void vupkhpx(Instruction code);
    void vupkhsb(Instruction code);
    void vupkhsh(Instruction code);
    void vupklpx(Instruction code);
    void vupklsb(Instruction code);
    void vupklsh(Instruction code);
    void vxor(Instruction code);
};

}  // namespace ppu
}  // namespace cpu
