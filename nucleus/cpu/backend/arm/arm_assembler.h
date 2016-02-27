/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/assembler.h"

namespace cpu {
namespace backend {
namespace arm {

/**
 * ARM Assembler
 * =============
 * Implements a JIT-assembler for the ARMv8 / AArch64 architecture.
 *
 * References:
 *  [1] ARM Architecture Reference Manual
 *  [2] ARMv8 Instruction Set Overview (PRD03-GENC-010197 15.0 / 11 November 2011)
 *  [3] VIXL: AArch64 Runtime Code Generation Library (Version 1.12)
 */

// Condition Codes. Section 4.3 @ Page 16 @ [1].
enum Condition {
    EQ = 0b0000,  // Equal                    Z == 1
    NE = 0b0001,  // Not equal                Z == 0         
    CS = 0b0010,  // Carry set                C == 1         
    CC = 0b0011,  // Carry clear              C == 0         
    MI = 0b0100,  // Negative                 N == 1         
    PL = 0b0101,  // Positive or zero         N == 0         
    VS = 0b0110,  // Overflow                 V == 1         
    VC = 0b0111,  // No overflow              V == 0         
    HI = 0b1000,  // Unsigned higher          C == 1 and Z == 0
    LS = 0b1001,  // Unsigned lower or same   C == 0 or  Z == 1
    GE = 0b1010,  // Greater or equal         N == V           
    LT = 0b1011,  // Less than                N != V           
    GT = 0b1100,  // Greater than             Z == 0 and N == V  
    LE = 0b1101,  // Less then or equal       Z == 1 or  N != V  
    AL = 0b1110,  // Always                   Any                 
    NV = 0b1111,  // Always                   Any
    
    // Aliases
    HS = CS,
    LO = CC,
};

enum StatusFlags {
    NONE       = 0,
    FLAG_N     = (1 << 3),
    FLAG_Z     = (1 << 2),
    FLAG_C     = (1 << 1),
    FLAG_V     = (1 << 0),
    FLAG_NZ    = FLAG_N | FLAG_Z,
    FLAG_NC    = FLAG_N | FLAG_C,
    FLAG_NV    = FLAG_N | FLAG_V,
    FLAG_ZC    = FLAG_Z | FLAG_C,
    FLAG_ZV    = FLAG_Z | FLAG_V,
    FLAG_CV    = FLAG_C | FLAG_V,
    FLAG_NZC   = FLAG_N | FLAG_Z | FLAG_C,
    FLAG_NZV   = FLAG_N | FLAG_Z | FLAG_V,
    FLAG_NCV   = FLAG_N | FLAG_C | FLAG_V,
    FLAG_ZCV   = FLAG_Z | FLAG_C | FLAG_V,
    FLAG_NZCV  = FLAG_N | FLAG_Z | FLAG_C | FLAG_V,
};

class Reg : public Register {
public:
    Reg(unsigned code, unsigned size) :
        Register(code, size, TYPE_INTEGER) {}

    Reg getWReg();
    Reg getXReg();
};

class VReg : public Register {
public:
    VReg(unsigned code, unsigned size) :
        Register(code, size, TYPE_VECTOR) {}
};

class ARMAssembler : public Assembler {
public:
    // Instruction Set Architecture
/*    void abs(const VReg& vd, const VReg& vn);
    void adc(const Reg& rd, const Reg& rn, const Operand& operand);
    void adcs(const Reg& rd, const Reg& rn, const Operand& operand);
    void add(const Reg& rd, const Reg& rn, const Operand& operand);
    void add(const VReg& vd, const VReg& vn, const VReg& vm);
    void addhn(const VReg& vd, const VReg& vn, const VReg& vm);
    void addhn2(const VReg& vd, const VReg& vn, const VReg& vm);
    void addp(const VReg& vd, const VReg& vn);
    void addp(const VReg& vd, const VReg& vn, const VReg& vm);
    void adds(const Reg& rd, const Reg& rn, const Operand& operand);
    void addv(const VReg& vd, const VReg& vn);
    void adr(const Reg& rd, int imm21);
    void adr(const Reg& rd, Label* label);
    void adrp(const Reg& rd, int imm21);
    void adrp(const Reg& rd, Label* label);
    void and_(const Reg& rd, const Reg& rn, const Operand& operand);
    void and_(const VReg& vd, const VReg& vn, const VReg& vm);
    void ands(const Reg& rd, const Reg& rn, const Operand& operand);
    void asr(const Reg& rd, const Reg& rn, unsigned shift); // Alias (sbfm)
    void asrv(const Reg& rd, const Reg& rn, const Reg& rm);
    void b(int imm19, Condition cond);
    void b(int imm26);
    void b(Label* label);
    void b(Label* label, Condition cond);
    void bfi(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (bfm)
    void bfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms);
    void bfxil(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (bfm)
    void bic(const Reg& rd, const Reg& rn, const Operand& operand);
    void bic(const VReg& vd, const int imm8, const int left_shift = 0);
    void bic(const VReg& vd, const VReg& vn, const VReg& vm);
    void bics(const Reg& rd, const Reg& rn, const Operand& operand);
    void bif(const VReg& vd, const VReg& vn, const VReg& vm);
    void bit(const VReg& vd, const VReg& vn, const VReg& vm);
    void bl(int imm26);
    void bl(Label* label);
    void blr(const Reg& xn);
    void br(const Reg& xn);
    void brk(int code);
    void bsl(const VReg& vd, const VReg& vn, const VReg& vm);
    void cbnz(const Reg& rt, int imm19);
    void cbnz(const Reg& rt, Label* label);
    void cbz(const Reg& rt, int imm19);
    void cbz(const Reg& rt, Label* label);
    void ccmn(const Reg& rn, const Operand& operand, StatusFlags nzcv, Condition cond);
    void ccmp(const Reg& rn, const Operand& operand, StatusFlags nzcv, Condition cond);
    void cinc(const Reg& rd, const Reg& rn, Condition cond);
    void cinv(const Reg& rd, const Reg& rn, Condition cond);
    void clrex(int imm4 = 0xf);
    void cls(const Reg& rd, const Reg& rn);(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void cls(const VReg& vd, const VReg& vn);
    void clz(const Reg& rd, const Reg& rn);
    void clz(const VReg& vd, const VReg& vn);
    void cmeq(const VReg& vd, const VReg& vn, const VReg& vm);
    void cmeq(const VReg& vd, const VReg& vn, int value);
    void cmge(const VReg& vd, const VReg& vn, const VReg& vm);
    void cmge(const VReg& vd, const VReg& vn, int value);
    void cmgt(const VReg& vd, const VReg& vn, const VReg& vm);
    void cmgt(const VReg& vd, const VReg& vn, int value);
    void cmhi(const VReg& vd, const VReg& vn, const VReg& vm);
    void cmhs(const VReg& vd, const VReg& vn, const VReg& vm);
    void cmle(const VReg& vd, const VReg& vn, int value);
    void cmlt(const VReg& vd, const VReg& vn, int value);
    void cmn(const Reg& rn, const Operand& operand);
    void cmp(const Reg& rn, const Operand& operand);
    void cmtst(const VReg& vd, const VReg& vn, const VReg& vm);
    void cneg(const Reg& rd, const Reg& rn, Condition cond);
    void cnt(const VReg& vd, const VReg& vn);
    void crc32b(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32cb(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32ch(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32cw(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32cx(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32h(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32w(const Reg& rd, const Reg& rn, const Reg& rm);
    void crc32x(const Reg& rd, const Reg& rn, const Reg& rm);
    void csel(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond);
    void cset(const Reg& rd, Condition cond);
    void csetm(const Reg& rd, Condition cond);
    void csinc(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond);
    void csinv(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond);
    void csneg(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond);
    void dc(DataCacheOp op, const Reg& rt);
    void dmb(BarrierDomain domain, BarrierType type);
    void dsb(BarrierDomain domain, BarrierType type);
    void dup(const VReg& vd, const Reg& rn);
    void dup(const VReg& vd, const VReg& vn, int vn_index);
    void eon(const Reg& rd, const Reg& rn, const Operand& operand);
    void eor(const Reg& rd, const Reg& rn, const Operand& operand);
    void eor(const VReg& vd, const VReg& vn, const VReg& vm);
    void ext(const VReg& vd, const VReg& vn, const VReg& vm, int index);
    void extr(const Reg& rd, const Reg& rn, const Reg& rm, unsigned lsb);
    void fabd(const VReg& vd, const VReg& vn, const VReg& vm);
    void fabs(const VReg& vd, const VReg& vn);
    void facge(const VReg& vd, const VReg& vn, const VReg& vm);
    void facgt(const VReg& vd, const VReg& vn, const VReg& vm);
    void fadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void faddp(const VReg& vd, const VReg& vn);
    void faddp(const VReg& vd, const VReg& vn, const VReg& vm);
    void fccmp(const VReg& vn, const VReg& vm, StatusFlags nzcv, Condition cond);
    void fccmpe(const VReg& vn, const VReg& vm, StatusFlags nzcv, Condition cond);
    void fcmeq(const VReg& vd, const VReg& vn, const VReg& vm);
    void fcmeq(const VReg& vd, const VReg& vn, double imm);
    void fcmge(const VReg& vd, const VReg& vn, const VReg& vm);
    void fcmge(const VReg& vd, const VReg& vn, double imm);
    void fcmgt(const VReg& vd, const VReg& vn, const VReg& vm);
    void fcmgt(const VReg& vd, const VReg& vn, double imm);
    void fcmle(const VReg& vd, const VReg& vn, double imm);
    void fcmlt(const VReg& vd, const VReg& vn, double imm);
    void fcmp(const VReg& vn, const VReg& vm);
    void fcmp(const VReg& vn, double value);
    void fcmpe(const VReg& vn, const VReg& vm);
    void fcmpe(const VReg& vn, double value);
    void fcsel(const VReg& vd, const VReg& vn, const VReg& vm, Condition cond);
    void fcvt(const VReg& vd, const VReg& vn);
    void fcvtas(const Reg& rd, const VReg& vn);
    void fcvtas(const VReg& vd, const VReg& vn);
    void fcvtau(const Reg& rd, const VReg& vn);
    void fcvtau(const VReg& vd, const VReg& vn);
    void fcvtl(const VReg& vd, const VReg& vn);
    void fcvtl2(const VReg& vd, const VReg& vn);
    void fcvtms(const Reg& rd, const VReg& vn);
    void fcvtms(const VReg& vd, const VReg& vn);
    void fcvtmu(const Reg& rd, const VReg& vn);
    void fcvtmu(const VReg& vd, const VReg& vn);
    void fcvtn(const VReg& vd, const VReg& vn);
    void fcvtn2(const VReg& vd, const VReg& vn);
    void fcvtns(const Reg& rd, const VReg& vn);
    void fcvtns(const VReg& rd, const VReg& vn);
    void fcvtnu(const Reg& rd, const VReg& vn);
    void fcvtnu(const VReg& rd, const VReg& vn);
    void fcvtps(const Reg& rd, const VReg& vn);
    void fcvtps(const VReg& vd, const VReg& vn);
    void fcvtpu(const Reg& rd, const VReg& vn);
    void fcvtpu(const VReg& vd, const VReg& vn);
    void fcvtxn(const VReg& vd, const VReg& vn);
    void fcvtxn2(const VReg& vd, const VReg& vn);
    void fcvtzs(const Reg& rd, const VReg& vn, int fbits = 0);
    void fcvtzs(const VReg& vd, const VReg& vn, int fbits = 0);
    void fcvtzu(const Reg& rd, const VReg& vn, int fbits = 0);
    void fcvtzu(const VReg& vd, const VReg& vn, int fbits = 0);
    void fdiv(const VReg& vd, const VReg& fn, const VReg& vm);
    void fmadd(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va);
    void fmax(const VReg& vd, const VReg& fn, const VReg& vm);
    void fmaxnm(const VReg& vd, const VReg& fn, const VReg& vm);
    void fmaxnmp(const VReg& vd, const VReg& vn);
    void fmaxnmp(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmaxnmv(const VReg& vd, const VReg& vn);
    void fmaxp(const VReg& vd, const VReg& vn);
    void fmaxp(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmaxv(const VReg& vd, const VReg& vn);
    void fmin(const VReg& vd, const VReg& fn, const VReg& vm);
    void fminnm(const VReg& vd, const VReg& fn, const VReg& vm);
    void fminnmp(const VReg& vd, const VReg& vn);
    void fminnmp(const VReg& vd, const VReg& vn, const VReg& vm);
    void fminnmv(const VReg& vd, const VReg& vn);
    void fminp(const VReg& vd, const VReg& vn);
    void fminp(const VReg& vd, const VReg& vn, const VReg& vm);
    void fminv(const VReg& vd, const VReg& vn);
    void fmla(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmla(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void fmls(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmls(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void fmov(const Reg& rd, const VReg& fn);
    void fmov(const Reg& rd, const VReg& vn, int index);
    void fmov(const VReg& vd, const Reg& rn);
    void fmov(const VReg& vd, const VReg& fn);
    void fmov(const VReg& vd, float imm);
    void fmov(const VReg& vd, int index, const Reg& rn);
    void fmsub(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va);
    void fmul(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmul(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void fmulx(const VReg& vd, const VReg& vn, const VReg& vm);
    void fmulx(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void fneg(const VReg& vd, const VReg& vn);
    void fnmadd(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va);
    void fnmsub(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va);
    void fnmul(const VReg& vd, const VReg& vn, const VReg& vm);
    void frecpe(const VReg& vd, const VReg& vn);
    void frecps(const VReg& vd, const VReg& vn, const VReg& vm);
    void frecpx(const VReg& vd, const VReg& vn);
    void frinta(const VReg& vd, const VReg& vn);
    void frinti(const VReg& vd, const VReg& vn);
    void frintm(const VReg& vd, const VReg& vn);
    void frintn(const VReg& vd, const VReg& vn);
    void frintp(const VReg& vd, const VReg& vn);
    void frintx(const VReg& vd, const VReg& vn);
    void frintz(const VReg& vd, const VReg& vn);
    void frsqrte(const VReg& vd, const VReg& vn);
    void frsqrts(const VReg& vd, const VReg& vn, const VReg& vm);
    void fsqrt(const VReg& vd, const VReg& vn);
    void fsub(const VReg& vd, const VReg& vn, const VReg& vm);
    void hint(SystemHint code);
    void hlt(int code);
    void ic(InstructionCacheOp op, const Reg& rt);
    void ins(const VReg& vd, int vd_index, const Reg& rn);
    void ins(const VReg& vd, int vd_index, const VReg& vn, int vn_index);
    void isb();
    void ld1(const VReg& vt, const MemOperand& src);
    void ld1(const VReg& vt, const VReg& vt2, const MemOperand& src);
    void ld1(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src);
    void ld1(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src);
    void ld1(const VReg& vt, int lane, const MemOperand& src);
    void ld1r(const VReg& vt, const MemOperand& src);
    void ld2(const VReg& vt, const VReg& vt2, const MemOperand& src);
    void ld2(const VReg& vt, const VReg& vt2, int lane, const MemOperand& src);
    void ld2r(const VReg& vt, const VReg& vt2, const MemOperand& src);
    void ld3(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src);
    void ld3(const VReg& vt, const VReg& vt2, const VReg& vt3, int lane, const MemOperand& src);
    void ld3r(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src);
    void ld4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src);
    void ld4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, int lane, const MemOperand& src);
    void ld4r(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src);
    void ldar(const Reg& rt, const MemOperand& src);
    void ldarb(const Reg& rt, const MemOperand& src);
    void ldarh(const Reg& rt, const MemOperand& src);
    void ldaxp(const Reg& rt, const Reg& rt2, const MemOperand& src);
    void ldaxr(const Reg& rt, const MemOperand& src);
    void ldaxrb(const Reg& rt, const MemOperand& src);
    void ldaxrh(const Reg& rt, const MemOperand& src);
    void ldnp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& src);
    void ldp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& src);
    void ldpsw(const Reg& rt, const Reg& rt2, const MemOperand& src);
    void ldr(const CPURegister& rt, int imm19);
    void ldr(const CPURegister& rt, RawLiteral* literal);
    void ldrb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void ldrh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void ldrsb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void ldrsh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void ldrsw(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset);
    void ldrsw(const Reg& rt, int imm19);
    void ldrsw(const Reg& rt, RawLiteral* literal);
    void ldur(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldurb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldurh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldursb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldursh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldursw(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void ldxp(const Reg& rt, const Reg& rt2, const MemOperand& src);
    void ldxr(const Reg& rt, const MemOperand& src);
    void ldxrb(const Reg& rt, const MemOperand& src);
    void ldxrh(const Reg& rt, const MemOperand& src);
    void lsl(const Reg& rd, const Reg& rn, unsigned shift); // Alias (ubfm)
    void lslv(const Reg& rd, const Reg& rn, const Reg& rm);
    void lsr(const Reg& rd, const Reg& rn, unsigned shift); // Alias (ubfm)
    void lsrv(const Reg& rd, const Reg& rn, const Reg& rm);
    void madd(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void mla(const VReg& vd, const VReg& vn, const VReg& vm);
    void mla(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void mls(const VReg& vd, const VReg& vn, const VReg& vm);
    void mls(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void mneg(const Reg& rd, const Reg& rn, const Reg& rm);
    void mov(const Reg& rd, const Reg& rn);
    void mov(const Reg& rd, const VReg& vn, int vn_index);
    void mov(const VReg& vd, const VReg& vn);
    void mov(const VReg& vd, const VReg& vn, int vn_index);
    void mov(const VReg& vd, int vd_index, const Reg& rn);
    void mov(const VReg& vd, int vd_index, const VReg& vn, int vn_index);
    void movi(const VReg& vd, const uint64_t imm, Shift shift = LSL, const int shift_amount = 0);
    void movk(const Reg& rd, uint64_t imm, int shift = -1);
    void movn(const Reg& rd, uint64_t imm, int shift = -1);
    void movz(const Reg& rd, uint64_t imm, int shift = -1);
    void msr(SystemRegister sysreg, const Reg& rt);
    void msub(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void mul(const Reg& rd, const Reg& rn, const Reg& rm);
    void mul(const VReg& vd, const VReg& vn, const VReg& vm);
    void mul(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void mvn(const Reg& rd, const Operand& operand);(const Reg& rt, SystemRegister sysreg);
    void mvn(const VReg& vd, const VReg& vn);
    void mvni(const VReg& vd, const int imm8, Shift shift = LSL, const int shift_amount = 0);
    void neg(const Reg& rd, const Operand& operand);
    void neg(const VReg& vd, const VReg& vn);
    void negs(const Reg& rd, const Operand& operand);
    void ngc(const Reg& rd, const Operand& operand);
    void ngcs(const Reg& rd, const Operand& operand);
    void nop();
    void not_(const VReg& vd, const VReg& vn);
    void orn(const Reg& rd, const Reg& rn, const Operand& operand);
    void orn(const VReg& vd, const VReg& vn, const VReg& vm);
    void orr(const Reg& rd, const Reg& rn, const Operand& operand);
    void orr(const VReg& vd, const int imm8, const int left_shift = 0);
    void orr(const VReg& vd, const VReg& vn, const VReg& vm);
    void pmul(const VReg& vd, const VReg& vn, const VReg& vm);
    void pmull(const VReg& vd, const VReg& vn, const VReg& vm);
    void pmull2(const VReg& vd, const VReg& vn, const VReg& vm);
    void prfm(PrefetchOperation op, const MemOperand& addr, LoadStoreScalingOption option = PreferScaledOffset);
    void prfm(PrefetchOperation op, int imm19);
    void prfm(PrefetchOperation op, RawLiteral* literal);
    void prfum(PrefetchOperation op, const MemOperand& addr, LoadStoreScalingOption option = PreferUnscaledOffset);
    void raddhn(const VReg& vd, const VReg& vn, const VReg& vm);
    void raddhn2(const VReg& vd, const VReg& vn, const VReg& vm);
    void rbit(const Reg& rd, const Reg& rn);16-bit half words.
    void rbit(const VReg& vd, const VReg& vn);
    void ret(const Reg& xn = lr);
    void rev(const Reg& rd, const Reg& rn);
    void rev16(const Reg& rd, const Reg& rn);32-bit words.
    void rev16(const VReg& vd, const VReg& vn);
    void rev32(const Reg& rd, const Reg& rn);
    void rev32(const VReg& vd, const VReg& vn);
    void rev64(const VReg& vd, const VReg& vn);
    void ror(const Reg& rd, const Reg& rs, unsigned shift);
    void rorv(const Reg& rd, const Reg& rn, const Reg& rm);
    void rshrn(const VReg& vd, const VReg& vn, int shift);
    void rshrn2(const VReg& vd, const VReg& vn, int shift);
    void rsubhn(const VReg& vd, const VReg& vn, const VReg& vm);
    void rsubhn2(const VReg& vd, const VReg& vn, const VReg& vm);
    void saba(const VReg& vd, const VReg& vn, const VReg& vm);
    void sabal(const VReg& vd, const VReg& vn, const VReg& vm);
    void sabal2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sabd(const VReg& vd, const VReg& vn, const VReg& vm);
    void sabdl(const VReg& vd, const VReg& vn, const VReg& vm);
    void sabdl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sadalp(const VReg& vd, const VReg& vn);
    void saddl(const VReg& vd, const VReg& vn, const VReg& vm);
    void saddl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void saddlp(const VReg& vd, const VReg& vn);
    void saddlv(const VReg& vd, const VReg& vn);
    void saddw(const VReg& vd, const VReg& vn, const VReg& vm);
    void saddw2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sbc(const Reg& rd, const Reg& rn, const Operand& operand);
    void sbcs(const Reg& rd, const Reg& rn, const Operand& operand);
    void sbfiz(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (sbfm)
    void sbfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms);
    void sbfx(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (sbfm)
    void scvtf(const VReg& fd, const Reg& rn, int fbits = 0);
    void scvtf(const VReg& fd, const VReg& vn, int fbits = 0);
    void sdiv(const Reg& rd, const Reg& rn, const Reg& rm);
    void shadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void shl(const VReg& vd, const VReg& vn, int shift);
    void shll(const VReg& vd, const VReg& vn, int shift);
    void shll2(const VReg& vd, const VReg& vn, int shift);
    void shrn(const VReg& vd, const VReg& vn, int shift);
    void shrn2(const VReg& vd, const VReg& vn, int shift);
    void shsub(const VReg& vd, const VReg& vn, const VReg& vm);
    void sli(const VReg& vd, const VReg& vn, int shift);
    void smaddl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void smax(const VReg& vd, const VReg& vn, const VReg& vm);
    void smaxp(const VReg& vd, const VReg& vn, const VReg& vm);
    void smaxv(const VReg& vd, const VReg& vn);
    void smin(const VReg& vd, const VReg& vn, const VReg& vm);
    void sminp(const VReg& vd, const VReg& vn, const VReg& vm);
    void sminv(const VReg& vd, const VReg& vn);
    void smlal(const VReg& vd, const VReg& vn, const VReg& vm);
    void smlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void smlal2(const VReg& vd, const VReg& vn, const VReg& vm);
    void smlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void smlsl(const VReg& vd, const VReg& vn, const VReg& vm);
    void smlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void smlsl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void smlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void smov(const Reg& rd, const VReg& vn, int vn_index);
    void smsubl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void smulh(const Reg& xd, const Reg& xn, const Reg& xm);
    void smull(const Reg& rd, const Reg& rn, const Reg& rm);
    void smull(const VReg& vd, const VReg& vn, const VReg& vm);
    void smull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void smull2(const VReg& vd, const VReg& vn, const VReg& vm);
    void smull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqabs(const VReg& vd, const VReg& vn);
    void sqadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmlal(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmlal2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmlsl(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmlsl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmulh(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmulh(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmull(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqdmull2(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqdmull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqneg(const VReg& vd, const VReg& vn);
    void sqrdmulh(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqrdmulh(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void sqrshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqrshrn(const VReg& vd, const VReg& vn, int shift);
    void sqrshrn2(const VReg& vd, const VReg& vn, int shift);
    void sqrshrun(const VReg& vd, const VReg& vn, int shift);
    void sqrshrun2(const VReg& vd, const VReg& vn, int shift);
    void sqshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqshl(const VReg& vd, const VReg& vn, int shift);
    void sqshlu(const VReg& vd, const VReg& vn, int shift);
    void sqshrn(const VReg& vd, const VReg& vn, int shift);
    void sqshrn2(const VReg& vd, const VReg& vn, int shift);
    void sqshrun(const VReg& vd, const VReg& vn, int shift);
    void sqshrun2(const VReg& vd, const VReg& vn, int shift);
    void sqsub(const VReg& vd, const VReg& vn, const VReg& vm);
    void sqxtn(const VReg& vd, const VReg& vn);
    void sqxtn2(const VReg& vd, const VReg& vn);
    void sqxtun(const VReg& vd, const VReg& vn);
    void sqxtun2(const VReg& vd, const VReg& vn);
    void srhadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void sri(const VReg& vd, const VReg& vn, int shift);
    void srshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void srshr(const VReg& vd, const VReg& vn, int shift);
    void srsra(const VReg& vd, const VReg& vn, int shift);
    void sshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void sshll(const VReg& vd, const VReg& vn, int shift);
    void sshll2(const VReg& vd, const VReg& vn, int shift);
    void sshr(const VReg& vd, const VReg& vn, int shift);
    void ssra(const VReg& vd, const VReg& vn, int shift);
    void ssubl(const VReg& vd, const VReg& vn, const VReg& vm);
    void ssubl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void ssubw(const VReg& vd, const VReg& vn, const VReg& vm);
    void ssubw2(const VReg& vd, const VReg& vn, const VReg& vm);
    void st1(const VReg& vt, const MemOperand& src);
    void st1(const VReg& vt, const VReg& vt2, const MemOperand& src);
    void st1(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src);
    void st1(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src);
    void st1(const VReg& vt, int lane, const MemOperand& src);
    void st2(const VReg& vt, const VReg& vt2, const MemOperand& src);
    void st2(const VReg& vt, const VReg& vt2, int lane, const MemOperand& src);
    void st3(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src);
    void st3(const VReg& vt, const VReg& vt2, const VReg& vt3, int lane, const MemOperand& src);
    void st4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src);
    void st4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, int lane, const MemOperand& src);
    void stlr(const Reg& rt, const MemOperand& dst);
    void stlrb(const Reg& rt, const MemOperand& dst);
    void stlrh(const Reg& rt, const MemOperand& dst);
    void stlxp(const Reg& rs, const Reg& rt, const Reg& rt2, const MemOperand& dst);
    void stlxr(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void stlxrb(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void stlxrh(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void stnp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& dst);
    void stp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& dst);
    void str(const CPURegister& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset);
    void strb(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset);
    void strh(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset);
    void stur(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset);
    void sturb(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferUnscaledOffset);
    void sturh(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferUnscaledOffset);
    void stxp(const Reg& rs, const Reg& rt, const Reg& rt2, const MemOperand& dst);
    void stxr(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void stxrb(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void stxrh(const Reg& rs, const Reg& rt, const MemOperand& dst);
    void sub(const Reg& rd, const Reg& rn, const Operand& operand);
    void sub(const VReg& vd, const VReg& vn, const VReg& vm);
    void subhn(const VReg& vd, const VReg& vn, const VReg& vm);
    void subhn2(const VReg& vd, const VReg& vn, const VReg& vm);
    void subs(const Reg& rd, const Reg& rn, const Operand& operand);
    void suqadd(const VReg& vd, const VReg& vn);
    void svc(int code);
    void sxtb(const Reg& rd, const Reg& rn); // Alias (sbfm)
    void sxth(const Reg& rd, const Reg& rn); // Alias (sbfm)
    void sxtl(const VReg& vd, const VReg& vn);
    void sxtl2(const VReg& vd, const VReg& vn);
    void sxtw(const Reg& rd, const Reg& rn); // Alias (sbfm)
    void sys(int op, const Reg& rt = xzr);
    void sys(int op1, int crn, int crm, int op2, const Reg& rt = xzr);
    void tbl(const VReg& vd, const VReg& vn, const VReg& vm);
    void tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vm);
    void tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vm);
    void tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vn4, const VReg& vm);
    void tbnz(const Reg& rt, unsigned bit_pos, int imm14);
    void tbnz(const Reg& rt, unsigned bit_pos, Label* label);
    void tbx(const VReg& vd, const VReg& vn, const VReg& vm);
    void tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vm);
    void tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vm);
    void tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vn4, const VReg& vm);
    void tbz(const Reg& rt, unsigned bit_pos, int imm14);
    void tbz(const Reg& rt, unsigned bit_pos, Label* label);
    void trn1(const VReg& vd, const VReg& vn, const VReg& vm);
    void trn2(const VReg& vd, const VReg& vn, const VReg& vm);
    void tst(const Reg& rn, const Operand& operand);
    void uaba(const VReg& vd, const VReg& vn, const VReg& vm);
    void uabal(const VReg& vd, const VReg& vn, const VReg& vm);
    void uabal2(const VReg& vd, const VReg& vn, const VReg& vm);
    void uabd(const VReg& vd, const VReg& vn, const VReg& vm);
    void uabdl(const VReg& vd, const VReg& vn, const VReg& vm);
    void uabdl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void uadalp(const VReg& vd, const VReg& vn);
    void uaddl(const VReg& vd, const VReg& vn, const VReg& vm);
    void uaddl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void uaddlp(const VReg& vd, const VReg& vn);
    void uaddlv(const VReg& vd, const VReg& vn);
    void uaddw(const VReg& vd, const VReg& vn, const VReg& vm);
    void uaddw2(const VReg& vd, const VReg& vn, const VReg& vm);
    void ubfiz(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (ubfm)
    void ubfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms);
    void ubfx(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width); // Alias (ubfm)
    void ucvtf(const VReg& fd, const Reg& rn, int fbits = 0);
    void ucvtf(const VReg& fd, const VReg& vn, int fbits = 0);
    void udiv(const Reg& rd, const Reg& rn, const Reg& rm);
    void uhadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void uhsub(const VReg& vd, const VReg& vn, const VReg& vm);
    void umaddl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void umax(const VReg& vd, const VReg& vn, const VReg& vm);
    void umaxp(const VReg& vd, const VReg& vn, const VReg& vm);
    void umaxv(const VReg& vd, const VReg& vn);
    void umin(const VReg& vd, const VReg& vn, const VReg& vm);
    void uminp(const VReg& vd, const VReg& vn, const VReg& vm);
    void uminv(const VReg& vd, const VReg& vn);
    void umlal(const VReg& vd, const VReg& vn, const VReg& vm);
    void umlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void umlal2(const VReg& vd, const VReg& vn, const VReg& vm);
    void umlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void umlsl(const VReg& vd, const VReg& vn, const VReg& vm);
    void umlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void umlsl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void umlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void umov(const Reg& rd, const VReg& vn, int vn_index);
    void umsubl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra);
    void umulh(const Reg& xd, const Reg& xn, const Reg& xm);
    void umull(const Reg& rd, const Reg& rn, const Reg& rm);
    void umull(const VReg& vd, const VReg& vn, const VReg& vm);
    void umull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void umull2(const VReg& vd, const VReg& vn, const VReg& vm);
    void umull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index);
    void uqadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void uqrshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void uqrshrn(const VReg& vd, const VReg& vn, int shift);
    void uqrshrn2(const VReg& vd, const VReg& vn, int shift);
    void uqshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void uqshl(const VReg& vd, const VReg& vn, int shift);
    void uqshrn(const VReg& vd, const VReg& vn, int shift);
    void uqshrn2(const VReg& vd, const VReg& vn, int shift);
    void uqsub(const VReg& vd, const VReg& vn, const VReg& vm);
    void uqxtn(const VReg& vd, const VReg& vn);
    void uqxtn2(const VReg& vd, const VReg& vn);
    void urecpe(const VReg& vd, const VReg& vn);
    void urhadd(const VReg& vd, const VReg& vn, const VReg& vm);
    void urshl(const VReg& vd, const VReg& vn, const VReg& vm);
    void urshr(const VReg& vd, const VReg& vn, int shift);
    void ursqrte(const VReg& vd, const VReg& vn);
    void ursra(const VReg& vd, const VReg& vn, int shift);
    void ushl(const VReg& vd, const VReg& vn, const VReg& vm);
    void ushll(const VReg& vd, const VReg& vn, int shift);
    void ushll2(const VReg& vd, const VReg& vn, int shift);
    void ushr(const VReg& vd, const VReg& vn, int shift);
    void usqadd(const VReg& vd, const VReg& vn);
    void usra(const VReg& vd, const VReg& vn, int shift);
    void usubl(const VReg& vd, const VReg& vn, const VReg& vm);
    void usubl2(const VReg& vd, const VReg& vn, const VReg& vm);
    void usubw(const VReg& vd, const VReg& vn, const VReg& vm);
    void usubw2(const VReg& vd, const VReg& vn, const VReg& vm);
    void uxtb(const Reg& rd, const Reg& rn); // Alias (ubfm)
    void uxth(const Reg& rd, const Reg& rn); // Alias (ubfm)
    void uxtl(const VReg& vd, const VReg& vn);
    void uxtl2(const VReg& vd, const VReg& vn);
    void uxtw(const Reg& rd, const Reg& rn); // Alias (ubfm)
    void uzp1(const VReg& vd, const VReg& vn, const VReg& vm);
    void uzp2(const VReg& vd, const VReg& vn, const VReg& vm);
    void xtn(const VReg& vd, const VReg& vn);
    void xtn2(const VReg& vd, const VReg& vn);
    void zip1(const VReg& vd, const VReg& vn, const VReg& vm);
    void zip2(const VReg& vd, const VReg& vn, const VReg& vm);*/
};

}  // namespace arm
}  // namespace backend
}  // namespace cpu
