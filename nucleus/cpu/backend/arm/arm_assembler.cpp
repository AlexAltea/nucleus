/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "arm_assembler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace backend {
namespace arm {

// Instruction set architecture
/*void ARMAssembler::abs(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::adc(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::adcs(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::add(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::add(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::addhn(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::addhn2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::addp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::addp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::adds(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::addv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::adr(const Reg& rd, int imm21) {
}
void ARMAssembler::adr(const Reg& rd, Label* label) {
}
void ARMAssembler::adrp(const Reg& rd, int imm21) {
}
void ARMAssembler::adrp(const Reg& rd, Label* label) {
}
void ARMAssembler::and_(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::and_(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ands(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::asr(const Reg& rd, const Reg& rn, unsigned shift) {
    assert_true(shift < rd.size);
    sbfm(rd, rn, shift, rd.size - 1);
}
void ARMAssembler::asrv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::b(int imm19, Condition cond) {
}
void ARMAssembler::b(int imm26) {
}
void ARMAssembler::b(Label* label) {
}
void ARMAssembler::b(Label* label, Condition cond) {
}
void ARMAssembler::bfi(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    bfm(rd, rn, (rd.size - lsb) & (rd.size - 1), width - 1);
}
void ARMAssembler::bfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms) {
}
void ARMAssembler::bfxil(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    bfm(rd, rn, lsb, lsb + width - 1);
}
void ARMAssembler::bic(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::bic(const VReg& vd, const int imm8, const int left_shift = 0) {
}
void ARMAssembler::bic(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::bics(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::bif(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::bit(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::bl(int imm26) {
}
void ARMAssembler::bl(Label* label) {
}
void ARMAssembler::blr(const Reg& xn) {
}
void ARMAssembler::br(const Reg& xn) {
}
void ARMAssembler::brk(int code) {
}
void ARMAssembler::bsl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cbnz(const Reg& rt, int imm19) {
}
void ARMAssembler::cbnz(const Reg& rt, Label* label) {
}
void ARMAssembler::cbz(const Reg& rt, int imm19) {
}
void ARMAssembler::cbz(const Reg& rt, Label* label) {
}
void ARMAssembler::ccmn(const Reg& rn, const Operand& operand, StatusFlags nzcv, Condition cond) {
}
void ARMAssembler::ccmp(const Reg& rn, const Operand& operand, StatusFlags nzcv, Condition cond) {
}
void ARMAssembler::cinc(const Reg& rd, const Reg& rn, Condition cond) {
}
void ARMAssembler::cinv(const Reg& rd, const Reg& rn, Condition cond) {
}
void ARMAssembler::clrex(int imm4 = 0xf) {
}
void ARMAssembler::cls(const Reg& rd, const Reg& rn) {
}(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::cls(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::clz(const Reg& rd, const Reg& rn) {
}
void ARMAssembler::clz(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::cmeq(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cmeq(const VReg& vd, const VReg& vn, int value) {
}
void ARMAssembler::cmge(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cmge(const VReg& vd, const VReg& vn, int value) {
}
void ARMAssembler::cmgt(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cmgt(const VReg& vd, const VReg& vn, int value) {
}
void ARMAssembler::cmhi(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cmhs(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cmle(const VReg& vd, const VReg& vn, int value) {
}
void ARMAssembler::cmlt(const VReg& vd, const VReg& vn, int value) {
}
void ARMAssembler::cmn(const Reg& rn, const Operand& operand) {
}
void ARMAssembler::cmp(const Reg& rn, const Operand& operand) {
}
void ARMAssembler::cmtst(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::cneg(const Reg& rd, const Reg& rn, Condition cond) {
}
void ARMAssembler::cnt(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::crc32b(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32cb(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32ch(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32cw(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32cx(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32h(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32w(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::crc32x(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::csel(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond) {
}
void ARMAssembler::cset(const Reg& rd, Condition cond) {
}
void ARMAssembler::csetm(const Reg& rd, Condition cond) {
}
void ARMAssembler::csinc(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond) {
}
void ARMAssembler::csinv(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond) {
}
void ARMAssembler::csneg(const Reg& rd, const Reg& rn, const Reg& rm, Condition cond) {
}
void ARMAssembler::dc(DataCacheOp op, const Reg& rt) {
}
void ARMAssembler::dmb(BarrierDomain domain, BarrierType type) {
}
void ARMAssembler::dsb(BarrierDomain domain, BarrierType type) {
}
void ARMAssembler::dup(const VReg& vd, const Reg& rn) {
}
void ARMAssembler::dup(const VReg& vd, const VReg& vn, int vn_index) {
}
void ARMAssembler::eon(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::eor(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::eor(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ext(const VReg& vd, const VReg& vn, const VReg& vm, int index) {
}
void ARMAssembler::extr(const Reg& rd, const Reg& rn, const Reg& rm, unsigned lsb) {
}
void ARMAssembler::fabd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fabs(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::facge(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::facgt(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::faddp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::faddp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fccmp(const VReg& vn, const VReg& vm, StatusFlags nzcv, Condition cond) {
}
void ARMAssembler::fccmpe(const VReg& vn, const VReg& vm, StatusFlags nzcv, Condition cond) {
}
void ARMAssembler::fcmeq(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fcmeq(const VReg& vd, const VReg& vn, double imm) {
}
void ARMAssembler::fcmge(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fcmge(const VReg& vd, const VReg& vn, double imm) {
}
void ARMAssembler::fcmgt(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fcmgt(const VReg& vd, const VReg& vn, double imm) {
}
void ARMAssembler::fcmle(const VReg& vd, const VReg& vn, double imm) {
}
void ARMAssembler::fcmlt(const VReg& vd, const VReg& vn, double imm) {
}
void ARMAssembler::fcmp(const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fcmp(const VReg& vn, double value) {
}
void ARMAssembler::fcmpe(const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fcmpe(const VReg& vn, double value) {
}
void ARMAssembler::fcsel(const VReg& vd, const VReg& vn, const VReg& vm, Condition cond) {
}
void ARMAssembler::fcvt(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtas(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtas(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtau(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtau(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtl(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtl2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtms(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtms(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtmu(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtmu(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtn2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtns(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtns(const VReg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtnu(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtnu(const VReg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtps(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtps(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtpu(const Reg& rd, const VReg& vn) {
}
void ARMAssembler::fcvtpu(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtxn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtxn2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fcvtzs(const Reg& rd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::fcvtzs(const VReg& vd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::fcvtzu(const Reg& rd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::fcvtzu(const VReg& vd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::fdiv(const VReg& vd, const VReg& fn, const VReg& vm) {
}
void ARMAssembler::fmadd(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va) {
}
void ARMAssembler::fmax(const VReg& vd, const VReg& fn, const VReg& vm) {
}
void ARMAssembler::fmaxnm(const VReg& vd, const VReg& fn, const VReg& vm) {
}
void ARMAssembler::fmaxnmp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fmaxnmp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmaxnmv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fmaxp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fmaxp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmaxv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fmin(const VReg& vd, const VReg& fn, const VReg& vm) {
}
void ARMAssembler::fminnm(const VReg& vd, const VReg& fn, const VReg& vm) {
}
void ARMAssembler::fminnmp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fminnmp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fminnmv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fminp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fminp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fminv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fmla(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmla(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::fmls(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmls(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::fmov(const Reg& rd, const VReg& fn) {
}
void ARMAssembler::fmov(const Reg& rd, const VReg& vn, int index) {
}
void ARMAssembler::fmov(const VReg& vd, const Reg& rn) {
}
void ARMAssembler::fmov(const VReg& vd, const VReg& fn) {
}
void ARMAssembler::fmov(const VReg& vd, float imm) {
}
void ARMAssembler::fmov(const VReg& vd, int index, const Reg& rn) {
}
void ARMAssembler::fmsub(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va) {
}
void ARMAssembler::fmul(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmul(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::fmulx(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fmulx(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::fneg(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fnmadd(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va) {
}
void ARMAssembler::fnmsub(const VReg& vd, const VReg& vn, const VReg& vm, const VReg& va) {
}
void ARMAssembler::fnmul(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::frecpe(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frecps(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::frecpx(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frinta(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frinti(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frintm(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frintn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frintp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frintx(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frintz(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frsqrte(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::frsqrts(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::fsqrt(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::fsub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::hint(SystemHint code) {
}
void ARMAssembler::hlt(int code) {
}
void ARMAssembler::ic(InstructionCacheOp op, const Reg& rt) {
}
void ARMAssembler::ins(const VReg& vd, int vd_index, const Reg& rn) {
}
void ARMAssembler::ins(const VReg& vd, int vd_index, const VReg& vn, int vn_index) {
}
void ARMAssembler::isb() {
}
void ARMAssembler::ld1(const VReg& vt, const MemOperand& src) {
}
void ARMAssembler::ld1(const VReg& vt, const VReg& vt2, const MemOperand& src) {
}
void ARMAssembler::ld1(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src) {
}
void ARMAssembler::ld1(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src) {
}
void ARMAssembler::ld1(const VReg& vt, int lane, const MemOperand& src) {
}
void ARMAssembler::ld1r(const VReg& vt, const MemOperand& src) {
}
void ARMAssembler::ld2(const VReg& vt, const VReg& vt2, const MemOperand& src) {
}
void ARMAssembler::ld2(const VReg& vt, const VReg& vt2, int lane, const MemOperand& src) {
}
void ARMAssembler::ld2r(const VReg& vt, const VReg& vt2, const MemOperand& src) {
}
void ARMAssembler::ld3(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src) {
}
void ARMAssembler::ld3(const VReg& vt, const VReg& vt2, const VReg& vt3, int lane, const MemOperand& src) {
}
void ARMAssembler::ld3r(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src) {
}
void ARMAssembler::ld4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src) {
}
void ARMAssembler::ld4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, int lane, const MemOperand& src) {
}
void ARMAssembler::ld4r(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src) {
}
void ARMAssembler::ldar(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldarb(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldarh(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldaxp(const Reg& rt, const Reg& rt2, const MemOperand& src) {
}
void ARMAssembler::ldaxr(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldaxrb(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldaxrh(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldnp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& src) {
}
void ARMAssembler::ldp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& src) {
}
void ARMAssembler::ldpsw(const Reg& rt, const Reg& rt2, const MemOperand& src) {
}
void ARMAssembler::ldr(const CPURegister& rt, int imm19) {
}
void ARMAssembler::ldr(const CPURegister& rt, RawLiteral* literal) {
}
void ARMAssembler::ldrb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::ldrh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::ldrsb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::ldrsh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::ldrsw(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::ldrsw(const Reg& rt, int imm19) {
}
void ARMAssembler::ldrsw(const Reg& rt, RawLiteral* literal) {
}
void ARMAssembler::ldur(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldurb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldurh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldursb(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldursh(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldursw(const Reg& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::ldxp(const Reg& rt, const Reg& rt2, const MemOperand& src) {
}
void ARMAssembler::ldxr(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldxrb(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::ldxrh(const Reg& rt, const MemOperand& src) {
}
void ARMAssembler::lsl(const Reg& rd, const Reg& rn, unsigned shift) {
    assert_true(shift < rd.size);
    ubfm(rd, rn, (rd.size - shift) % rd.size, rd.size - shift - 1);
}
void ARMAssembler::lslv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::lsr(const Reg& rd, const Reg& rn, unsigned shift) {
    assert_true(shift < rd.size);
    ubfm(rd, rn, shift, rd.size - 1);
}
void ARMAssembler::lsrv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::madd(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::mla(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::mla(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::mls(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::mls(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::mneg(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::mov(const Reg& rd, const Reg& rn) {
}
void ARMAssembler::mov(const Reg& rd, const VReg& vn, int vn_index) {
}
void ARMAssembler::mov(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::mov(const VReg& vd, const VReg& vn, int vn_index) {
}
void ARMAssembler::mov(const VReg& vd, int vd_index, const Reg& rn) {
}
void ARMAssembler::mov(const VReg& vd, int vd_index, const VReg& vn, int vn_index) {
}
void ARMAssembler::movi(const VReg& vd, const uint64_t imm, Shift shift = LSL, const int shift_amount = 0) {
}
void ARMAssembler::movk(const Reg& rd, uint64_t imm, int shift = -1) {
}
void ARMAssembler::movn(const Reg& rd, uint64_t imm, int shift = -1) {
}
void ARMAssembler::movz(const Reg& rd, uint64_t imm, int shift = -1) {
}
void ARMAssembler::msr(SystemRegister sysreg, const Reg& rt) {
}
void ARMAssembler::msub(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::mul(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::mul(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::mul(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::mvn(const Reg& rd, const Operand& operand) {
}(const Reg& rt, SystemRegister sysreg) {
}
void ARMAssembler::mvn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::mvni(const VReg& vd, const int imm8, Shift shift = LSL, const int shift_amount = 0) {
}
void ARMAssembler::neg(const Reg& rd, const Operand& operand) {
}
void ARMAssembler::neg(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::negs(const Reg& rd, const Operand& operand) {
}
void ARMAssembler::ngc(const Reg& rd, const Operand& operand) {
}
void ARMAssembler::ngcs(const Reg& rd, const Operand& operand) {
}
void ARMAssembler::nop() {
}
void ARMAssembler::not_(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::orn(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::orn(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::orr(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::orr(const VReg& vd, const int imm8, const int left_shift = 0) {
}
void ARMAssembler::orr(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::pmul(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::pmull(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::pmull2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::prfm(PrefetchOperation op, const MemOperand& addr, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::prfm(PrefetchOperation op, int imm19) {
}
void ARMAssembler::prfm(PrefetchOperation op, RawLiteral* literal) {
}
void ARMAssembler::prfum(PrefetchOperation op, const MemOperand& addr, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::raddhn(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::raddhn2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::rbit(const Reg& rd, const Reg& rn) {
}16-bit half words.
void ARMAssembler::rbit(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::ret(const Reg& xn = lr) {
}
void ARMAssembler::rev(const Reg& rd, const Reg& rn) {
}
void ARMAssembler::rev16(const Reg& rd, const Reg& rn) {
}32-bit words.
void ARMAssembler::rev16(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::rev32(const Reg& rd, const Reg& rn) {
}
void ARMAssembler::rev32(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::rev64(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::ror(const Reg& rd, const Reg& rs, unsigned shift) {
}
void ARMAssembler::rorv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::rshrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::rshrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::rsubhn(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::rsubhn2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::saba(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sabal(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sabal2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sabd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sabdl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sabdl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sadalp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::saddl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::saddl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::saddlp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::saddlv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::saddw(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::saddw2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sbc(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::sbcs(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::sbfiz(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    sbfm(rd, rn, (rd.size - lsb) & (rd.size - 1), width - 1);
}
void ARMAssembler::sbfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms) {
}
void ARMAssembler::sbfx(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    sbfm(rd, rn, lsb, lsb + width - 1);
}
void ARMAssembler::scvtf(const VReg& fd, const Reg& rn, int fbits = 0) {
}
void ARMAssembler::scvtf(const VReg& fd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::sdiv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::shadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::shl(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::shll(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::shll2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::shrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::shrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::shsub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sli(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::smaddl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::smax(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smaxp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smaxv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::smin(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sminp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sminv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::smlal(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::smlal2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::smlsl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::smlsl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::smov(const Reg& rd, const VReg& vn, int vn_index) {
}
void ARMAssembler::smsubl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::smulh(const Reg& xd, const Reg& xn, const Reg& xm) {
}
void ARMAssembler::smull(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::smull(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::smull2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::smull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqabs(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sqadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmlal(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmlal2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmlsl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmlsl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmulh(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmulh(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmull(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqdmull2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqdmull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqneg(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sqrdmulh(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqrdmulh(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::sqrshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqrshrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqrshrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqrshrun(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqrshrun2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqshl(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshlu(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshrun(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqshrun2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sqsub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sqxtn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sqxtn2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sqxtun(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sqxtun2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::srhadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sri(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::srshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::srshr(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::srsra(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::sshll(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sshll2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::sshr(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ssra(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ssubl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ssubl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ssubw(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ssubw2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::st1(const VReg& vt, const MemOperand& src) {
}
void ARMAssembler::st1(const VReg& vt, const VReg& vt2, const MemOperand& src) {
}
void ARMAssembler::st1(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src) {
}
void ARMAssembler::st1(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src) {
}
void ARMAssembler::st1(const VReg& vt, int lane, const MemOperand& src) {
}
void ARMAssembler::st2(const VReg& vt, const VReg& vt2, const MemOperand& src) {
}
void ARMAssembler::st2(const VReg& vt, const VReg& vt2, int lane, const MemOperand& src) {
}
void ARMAssembler::st3(const VReg& vt, const VReg& vt2, const VReg& vt3, const MemOperand& src) {
}
void ARMAssembler::st3(const VReg& vt, const VReg& vt2, const VReg& vt3, int lane, const MemOperand& src) {
}
void ARMAssembler::st4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, const MemOperand& src) {
}
void ARMAssembler::st4(const VReg& vt, const VReg& vt2, const VReg& vt3, const VReg& vt4, int lane, const MemOperand& src) {
}
void ARMAssembler::stlr(const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stlrb(const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stlrh(const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stlxp(const Reg& rs, const Reg& rt, const Reg& rt2, const MemOperand& dst) {
}
void ARMAssembler::stlxr(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stlxrb(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stlxrh(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stnp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& dst) {
}
void ARMAssembler::stp(const CPURegister& rt, const CPURegister& rt2, const MemOperand& dst) {
}
void ARMAssembler::str(const CPURegister& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::strb(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::strh(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferScaledOffset) {
}
void ARMAssembler::stur(const CPURegister& rt, const MemOperand& src, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::sturb(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::sturh(const Reg& rt, const MemOperand& dst, LoadStoreScalingOption option = PreferUnscaledOffset) {
}
void ARMAssembler::stxp(const Reg& rs, const Reg& rt, const Reg& rt2, const MemOperand& dst) {
}
void ARMAssembler::stxr(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stxrb(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::stxrh(const Reg& rs, const Reg& rt, const MemOperand& dst) {
}
void ARMAssembler::sub(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::sub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::subhn(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::subhn2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::subs(const Reg& rd, const Reg& rn, const Operand& operand) {
}
void ARMAssembler::suqadd(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::svc(int code) {
}
void ARMAssembler::sxtb(const Reg& rd, const Reg& rn) {
    sbfm(rd, rn, 0, 7);
}
void ARMAssembler::sxth(const Reg& rd, const Reg& rn) {
    sbfm(rd, rn, 0, 15);
}
void ARMAssembler::sxtl(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sxtl2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::sxtw(const Reg& rd, const Reg& rn) {
    sbfm(rd, rn, 0, 31);
}
void ARMAssembler::sys(int op, const Reg& rt = xzr) {
}
void ARMAssembler::sys(int op1, int crn, int crm, int op2, const Reg& rt = xzr) {
}
void ARMAssembler::tbl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vm) {
}
void ARMAssembler::tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vm) {
}
void ARMAssembler::tbl(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vn4, const VReg& vm) {
}
void ARMAssembler::tbnz(const Reg& rt, unsigned bit_pos, int imm14) {
}
void ARMAssembler::tbnz(const Reg& rt, unsigned bit_pos, Label* label) {
}
void ARMAssembler::tbx(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vm) {
}
void ARMAssembler::tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vm) {
}
void ARMAssembler::tbx(const VReg& vd, const VReg& vn, const VReg& vn2, const VReg& vn3, const VReg& vn4, const VReg& vm) {
}
void ARMAssembler::tbz(const Reg& rt, unsigned bit_pos, int imm14) {
}
void ARMAssembler::tbz(const Reg& rt, unsigned bit_pos, Label* label) {
}
void ARMAssembler::trn1(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::trn2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::tst(const Reg& rn, const Operand& operand) {
}
void ARMAssembler::uaba(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uabal(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uabal2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uabd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uabdl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uabdl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uadalp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uaddl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uaddl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uaddlp(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uaddlv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uaddw(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uaddw2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ubfiz(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    ubfm(rd, rn, (rd.size - lsb) & (rd.size - 1), width - 1);
}
void ARMAssembler::ubfm(const Reg& rd, const Reg& rn, unsigned immr, unsigned imms) {
}
void ARMAssembler::ubfx(const Reg& rd, const Reg& rn, unsigned lsb, unsigned width) {
    assert_true(width >= 1);
    assert_true(lsb + width <= rn.size);
    ubfm(rd, rn, lsb, lsb + width - 1);
}
void ARMAssembler::ucvtf(const VReg& fd, const Reg& rn, int fbits = 0) {
}
void ARMAssembler::ucvtf(const VReg& fd, const VReg& vn, int fbits = 0) {
}
void ARMAssembler::udiv(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::uhadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uhsub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umaddl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::umax(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umaxp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umaxv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::umin(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uminp(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uminv(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::umlal(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umlal(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::umlal2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umlal2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::umlsl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umlsl(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::umlsl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umlsl2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::umov(const Reg& rd, const VReg& vn, int vn_index) {
}
void ARMAssembler::umsubl(const Reg& rd, const Reg& rn, const Reg& rm, const Reg& ra) {
}
void ARMAssembler::umulh(const Reg& xd, const Reg& xn, const Reg& xm) {
}
void ARMAssembler::umull(const Reg& rd, const Reg& rn, const Reg& rm) {
}
void ARMAssembler::umull(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umull(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::umull2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::umull2(const VReg& vd, const VReg& vn, const VReg& vm, int vm_index) {
}
void ARMAssembler::uqadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uqrshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uqrshrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::uqrshrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::uqshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uqshl(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::uqshrn(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::uqshrn2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::uqsub(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uqxtn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uqxtn2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::urecpe(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::urhadd(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::urshl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::urshr(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ursqrte(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::ursra(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ushl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::ushll(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ushll2(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::ushr(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::usqadd(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::usra(const VReg& vd, const VReg& vn, int shift) {
}
void ARMAssembler::usubl(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::usubl2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::usubw(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::usubw2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uxtb(const Reg& rd, const Reg& rn) {
    ubfm(rd, rn, 0, 7);
}
void ARMAssembler::uxth(const Reg& rd, const Reg& rn) {
    ubfm(rd, rn, 0, 15);
}
void ARMAssembler::uxtl(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uxtl2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::uxtw(const Reg& rd, const Reg& rn) {
    ubfm(rd, rn, 0, 31) {
}
void ARMAssembler::uzp1(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::uzp2(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::xtn(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::xtn2(const VReg& vd, const VReg& vn) {
}
void ARMAssembler::zip1(const VReg& vd, const VReg& vn, const VReg& vm) {
}
void ARMAssembler::zip2(const VReg& vd, const VReg& vn, const VReg& vm) {
}*/

}  // namespace arm
}  // namespace backend
}  // namespace cpu
