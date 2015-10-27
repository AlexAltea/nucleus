/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppc_assembler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace backend {
namespace ppc {

PPCAssembler::PPCAssembler(size_t codeSize, void* codeAddr) : Assembler(codeSize, codeAddr) {
}

void PPCAssembler::emit(U32 instruction) {
    auto* addr = static_cast<U32*>(curAddr);
    *addr = instruction;
    codeSize = 4;
    addr++;
}

// Emit instruction form
void PPCAssembler::emitFormI(U32 instruction, Operand li) {
    assert_true(imm % 4 == 0, "The destination cannot overwrite the 2 LSb of a I-Form instruction");
    emit(instruction | li);
}
void PPCAssembler::emitFormB(U32 instruction, Operand bo, Operand bi, Operand bd) {
    assert_true(imm % 4 == 0, "The destination cannot overwrite the 2 LSb of a B-Form instruction");
    const U32 boMask = (bo & 0x1F) << 21;
    const U32 biMask = (bi & 0x1F) << 16;
    emit(instruction | boMask | biMask | bd);
}
void PPCAssembler::emitFormD(U32 instruction, Operand d, Operand a, U16 imm) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    emit(instruction | rdMask | raMask | imm);
}
void PPCAssembler::emitFormDS(U32 instruction, Operand d, Operand a, U16 imm) {
    assert_true(imm % 4 == 0, "The immediate cannot overwrite the 2 LSb of a DS-Form instruction");
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    emit(instruction | rdMask | raMask | imm);
}
void PPCAssembler::emitFormX(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit(instruction | rdMask | raMask | rbMask);
}
void PPCAssembler::emitFormXL(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit(instruction | rdMask | raMask | rbMask);
}
void PPCAssembler::emitFormXFX(U32 instruction, Operand d, Operand spr) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 sprMask = (spr & 0x3FF) << 11;
    emit(instruction | rdMask | sprMask);
}
void PPCAssembler::emitFormXO(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit(instruction | rdMask | raMask | rbMask);
}

// PPC instructions
void PPCAssembler::add(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000214, rd, ra, rb); }
void PPCAssembler::add_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000215, rd, ra, rb); }
void PPCAssembler::addc(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000014, rd, ra, rb); }
void PPCAssembler::addc_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000015, rd, ra, rb); }
void PPCAssembler::addco(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000414, rd, ra, rb); }
void PPCAssembler::addco_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000415, rd, ra, rb); }
void PPCAssembler::adde(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000114, rd, ra, rb); }
void PPCAssembler::adde_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000115, rd, ra, rb); }
void PPCAssembler::addeo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000514, rd, ra, rb); }
void PPCAssembler::addeo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000515, rd, ra, rb); }
void PPCAssembler::addi(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x38000000, rd, ra, simm); }
void PPCAssembler::addic(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x30000000, rd, ra, simm); }
void PPCAssembler::addic_(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x34000000, rd, ra, simm); }
void PPCAssembler::addis(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x3C000000, rd, ra, simm); }
void PPCAssembler::addme(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D4, rd, ra, 0); }
void PPCAssembler::addme_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D5, rd, ra, 0); }
void PPCAssembler::addmeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D4, rd, ra, 0); }
void PPCAssembler::addmeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D5, rd, ra, 0); }
void PPCAssembler::addo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000614, rd, ra, rb); }
void PPCAssembler::addo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000615, rd, ra, rb); }
void PPCAssembler::addze(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000194, rd, ra, 0); }
void PPCAssembler::addze_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000195, rd, ra, 0); }
void PPCAssembler::addzeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000594, rd, ra, 0); }
void PPCAssembler::addzeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000595, rd, ra, 0); }
void PPCAssembler::and_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000038, rs, ra, rb); }
void PPCAssembler::and__(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000039, rs, ra, rb); }
void PPCAssembler::andc(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000078, rs, ra, rb); }
void PPCAssembler::andc_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000079, rs, ra, rb); }
void PPCAssembler::andi_(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x70000000, rs, ra, uimm); }
void PPCAssembler::andis_(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x74000000, rs, ra, uimm); }
void PPCAssembler::b(Label label) { emitFormI(0x48000000, 0/*TODO*/); }
void PPCAssembler::ba(Label label) { emitFormI(0x48000002, 0/*TODO*/); }
void PPCAssembler::bl(Label label) { emitFormI(0x48000001, 0/*TODO*/); }
void PPCAssembler::bla(Label label) { emitFormI(0x48000003, 0/*TODO*/); }
void PPCAssembler::bc(U8 bo, U8 bi, Label label) { emitFormB(0x40000000, bo, bi, 0/*TODO*/); }
void PPCAssembler::blt(RegCR cr, Label label) { bc(12, (4 * cr) + 0, label); }
void PPCAssembler::ble(RegCR cr, Label label) { bc( 4, (4 * cr) + 1, label); }
void PPCAssembler::beq(RegCR cr, Label label) { bc(12, (4 * cr) + 2, label); }
void PPCAssembler::bge(RegCR cr, Label label) { bc( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgt(RegCR cr, Label label) { bc(12, (4 * cr) + 1, label); }
void PPCAssembler::bne(RegCR cr, Label label) { bc( 4, (4 * cr) + 2, label); }
void PPCAssembler::bso(RegCR cr, Label label) { bc(12, (4 * cr) + 3, label); }
void PPCAssembler::bca(U8 bo, U8 bi, Label label) { emitFormB(0x40000002, bo, bi, 0/*TODO*/); }
void PPCAssembler::blta(RegCR cr, Label label) { bca(12, (4 * cr) + 0, label); }
void PPCAssembler::blea(RegCR cr, Label label) { bca( 4, (4 * cr) + 1, label); }
void PPCAssembler::beqa(RegCR cr, Label label) { bca(12, (4 * cr) + 2, label); }
void PPCAssembler::bgea(RegCR cr, Label label) { bca( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgta(RegCR cr, Label label) { bca(12, (4 * cr) + 1, label); }
void PPCAssembler::bnea(RegCR cr, Label label) { bca( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsoa(RegCR cr, Label label) { bca(12, (4 * cr) + 3, label); }
void PPCAssembler::bcl(U8 bo, U8 bi, Label label) { emitFormB(0x40000001, bo, bi, 0/*TODO*/); }
void PPCAssembler::bltl(RegCR cr, Label label) { bcl(12, (4 * cr) + 0, label); }
void PPCAssembler::blel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 1, label); }
void PPCAssembler::beql(RegCR cr, Label label) { bcl(12, (4 * cr) + 2, label); }
void PPCAssembler::bgel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgtl(RegCR cr, Label label) { bcl(12, (4 * cr) + 1, label); }
void PPCAssembler::bnel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsol(RegCR cr, Label label) { bcl(12, (4 * cr) + 3, label); }
void PPCAssembler::bcla(U8 bo, U8 bi, Label label) { emitFormB(0x40000003, bo, bi, 0/*TODO*/); }
void PPCAssembler::bltla(RegCR cr, Label label) { bcla(12, (4 * cr) + 0, label); }
void PPCAssembler::blela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 1, label); }
void PPCAssembler::beqla(RegCR cr, Label label) { bcla(12, (4 * cr) + 2, label); }
void PPCAssembler::bgela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgtla(RegCR cr, Label label) { bcla(12, (4 * cr) + 1, label); }
void PPCAssembler::bnela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsola(RegCR cr, Label label) { bcla(12, (4 * cr) + 3, label); }
void PPCAssembler::bcctr(U8 bo, U8 bi, U8 bh, Label label) { emitFormXL(0x4C000420, bo, bi, bh); }
void PPCAssembler::bcctrl(U8 bo, U8 bi, U8 bh, Label label) { emitFormXL(0x4C000421, bo, bi, bh); }
void PPCAssembler::bclr(U8 bo, U8 bi, U8 bh, Label label) { emitFormXL(0x4C000020, bo, bi, bh); }
void PPCAssembler::bclrl(U8 bo, U8 bi, U8 bh, Label label) { emitFormXL(0x4C000021, bo, bi, bh); }
void PPCAssembler::cmp(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmp(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpd(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpd(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpdi(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpdi(RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpi(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpi(RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpl(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpl(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpld(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpld(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpldi(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpldi(RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpli(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpli(RegGPR ra, S16 simm) {  }
void PPCAssembler::cmplw(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmplw(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmplwi(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmplwi(RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpw(RegCR crfd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpw(RegGPR ra, RegGPR rb) {  }
void PPCAssembler::cmpwi(RegCR crfd, RegGPR ra, S16 simm) {  }
void PPCAssembler::cmpwi(RegGPR ra, S16 simm) {  }
void PPCAssembler::cntlzd(RegGPR ra, RegGPR rs) { emitFormX(0x7C000074, rs, ra, 0); }
void PPCAssembler::cntlzd_(RegGPR ra, RegGPR rs) { emitFormX(0x7C000075, rs, ra, 0); }
void PPCAssembler::cntlzw(RegGPR ra, RegGPR rs) { emitFormX(0x7C000034, rs, ra, 0); }
void PPCAssembler::cntlzw_(RegGPR ra, RegGPR rs) { emitFormX(0x7C000035, rs, ra, 0); }
void PPCAssembler::crand(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000202, crfd, crfa, crfb); }
void PPCAssembler::crandc(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000102, crfd, crfa, crfb); }
void PPCAssembler::creqv(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000242, crfd, crfa, crfb); }
void PPCAssembler::crnand(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C0001C2, crfd, crfa, crfb); }
void PPCAssembler::crnor(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000042, crfd, crfa, crfb); }
void PPCAssembler::cror(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000382, crfd, crfa, crfb); }
void PPCAssembler::crorc(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000342, crfd, crfa, crfb); }
void PPCAssembler::crxor(U8 crfd, U8 crfa, U8 crfb) { emitFormXL(0x4C000182, crfd, crfa, crfb); }
void PPCAssembler::dcbf(RegGPR ra, RegGPR rb) { emitFormX(0x7C0000AC, 0, ra, rb); }
void PPCAssembler::dcbst(RegGPR ra, RegGPR rb) { emitFormX(0x7C00006C, 0, ra, rb); }
void PPCAssembler::dcbt() {  }
void PPCAssembler::dcbtst(RegGPR ra, RegGPR rb) { emitFormX(0x7C0001EC, 0, ra, rb); }
void PPCAssembler::dcbz(RegGPR ra, RegGPR rb) { emitFormX(0x7C0007EC, 0, ra, rb); }
void PPCAssembler::divd(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D2, rd, ra, rb); }
void PPCAssembler::divd_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D3, rd, ra, rb); }
void PPCAssembler::divdo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D2, rd, ra, rb); }
void PPCAssembler::divdo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D3, rd, ra, rb); }
void PPCAssembler::divdu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000392, rd, ra, rb); }
void PPCAssembler::divdu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000393, rd, ra, rb); }
void PPCAssembler::divduo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000792, rd, ra, rb); }
void PPCAssembler::divduo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000793, rd, ra, rb); }
void PPCAssembler::divw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D6, rd, ra, rb); }
void PPCAssembler::divw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D7, rd, ra, rb); }
void PPCAssembler::divwo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D6, rd, ra, rb); }
void PPCAssembler::divwo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D7, rd, ra, rb); }
void PPCAssembler::divwu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000396, rd, ra, rb); }
void PPCAssembler::divwu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000397, rd, ra, rb); }
void PPCAssembler::divwuo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000796, rd, ra, rb); }
void PPCAssembler::divwuo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000797, rd, ra, rb); }
void PPCAssembler::eciwx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00026C, rd, ra, rb); }
void PPCAssembler::ecowx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00036C, rs, ra, rb); }
void PPCAssembler::eieio() { emitFormX(0x7C0006AC, 0, 0, 0); }
void PPCAssembler::eqv(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000238, rs, ra, rb); }
void PPCAssembler::eqv_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000239, rs, ra, rb); }
void PPCAssembler::extsb(RegGPR ra, RegGPR rs) { emitFormX(0x7C000774, rs, ra, 0); }
void PPCAssembler::extsb_(RegGPR ra, RegGPR rs) { emitFormX(0x7C000775, rs, ra, 0); }
void PPCAssembler::extsh(RegGPR ra, RegGPR rs) { emitFormX(0x7C000734, rs, ra, 0); }
void PPCAssembler::extsh_(RegGPR ra, RegGPR rs) { emitFormX(0x7C000735, rs, ra, 0); }
void PPCAssembler::extsw(RegGPR ra, RegGPR rs) { emitFormX(0x7C0007B4, rs, ra, 0); }
void PPCAssembler::extsw_(RegGPR ra, RegGPR rs) { emitFormX(0x7C0007B5, rs, ra, 0); }
void PPCAssembler::fabs(RegFPR frd, RegFPR frb) { emitFormX(0xFC000210, frd, 0, frb); }
void PPCAssembler::fabs_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000211, frd, 0, frb); }
void PPCAssembler::fadd(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fadd_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fadds(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fadds_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fcfid(RegFPR frd, RegFPR frb) { emitFormX(0xFC00069C, frd, 0, frb); }
void PPCAssembler::fcfid_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00069D, frd, 0, frb); }
void PPCAssembler::fcmpo(RegCR crfd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fcmpu(RegCR crfd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fctid(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065C, frd, 0, frb); }
void PPCAssembler::fctid_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065D, frd, 0, frb); }
void PPCAssembler::fctidz(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065E, frd, 0, frb); }
void PPCAssembler::fctidz_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065F, frd, 0, frb); }
void PPCAssembler::fctiw(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001C, frd, 0, frb); }
void PPCAssembler::fctiw_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001D, frd, 0, frb); }
void PPCAssembler::fctiwz(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001E, frd, 0, frb); }
void PPCAssembler::fctiwz_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001F, frd, 0, frb); }
void PPCAssembler::fdiv(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fdiv_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fdivs(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fdivs_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmr(RegFPR frd, RegFPR frb) { emitFormX(0xFC000090, frd, 0, frb); }
void PPCAssembler::fmr_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000091, frd, 0, frb); }
void PPCAssembler::fmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fmul(RegFPR frd, RegFPR fra, RegFPR frc) {  }
void PPCAssembler::fmul_(RegFPR frd, RegFPR fra, RegFPR frc) {  }
void PPCAssembler::fmuls(RegFPR frd, RegFPR fra, RegFPR frc) {  }
void PPCAssembler::fmuls_(RegFPR frd, RegFPR fra, RegFPR frc) {  }
void PPCAssembler::fnabs(RegFPR frd, RegFPR frb) { emitFormX(0xFC000110, frd, 0, frb); }
void PPCAssembler::fnabs_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000111, frd, 0, frb); }
void PPCAssembler::fneg(RegFPR frd, RegFPR frb) { emitFormX(0xFC000050, frd, 0, frb); }
void PPCAssembler::fneg_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000051, frd, 0, frb); }
void PPCAssembler::fnmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fnmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fres(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fres_(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::frsp(RegFPR frd, RegFPR frb) { emitFormX(0xFC000018, frd, 0, frb); }
void PPCAssembler::frsp_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000019, frd, 0, frb); }
void PPCAssembler::frsqrte(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::frsqrte_(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fsel(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fsel_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) {  }
void PPCAssembler::fsqrt(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fsqrt_(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fsqrts(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fsqrts_(RegFPR frd, RegFPR frb) {  }
void PPCAssembler::fsub(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fsub_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fsubs(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::fsubs_(RegFPR frd, RegFPR fra, RegFPR frb) {  }
void PPCAssembler::icbi(RegGPR ra, RegGPR rb) { emitFormX(0x7C0007AC, 0, ra, rb); }
void PPCAssembler::isync() {  }
void PPCAssembler::lbz(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0x88000000, rd, ra, d); }
void PPCAssembler::lbzu(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0x8C000000, rd, ra, d); }
void PPCAssembler::lbzux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0000EE, rd, ra, rb); }
void PPCAssembler::lbzx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0000AE, rd, ra, rb); }
void PPCAssembler::ld(RegGPR rd, RegGPR ra, U16 ds) { emitFormDS(0xE8000000, rd, ra, ds); }
void PPCAssembler::ldarx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0000A8, rd, ra, rb); }
void PPCAssembler::ldbrx() {  }
void PPCAssembler::ldu(RegGPR rd, RegGPR ra, U16 ds) { emitFormDS(0xE8000001, rd, ra, ds); }
void PPCAssembler::ldux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00006A, rd, ra, rb); }
void PPCAssembler::ldx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00002A, rd, ra, rb); }
void PPCAssembler::lfd(RegFPR frd, RegGPR ra, U16 d) { emitFormD(0xC8000000, frd, ra, d); }
void PPCAssembler::lfdu(RegFPR frd, RegGPR ra, U16 d) { emitFormD(0xCC000000, frd, ra, d); }
void PPCAssembler::lfdux(RegFPR frd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0004EE, frd, ra, rb); }
void PPCAssembler::lfdx(RegFPR frd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0004AE, frd, ra, rb); }
void PPCAssembler::lfs(RegFPR frd, RegGPR ra, U16 d) { emitFormD(0xC0000000, frd, ra, d); }
void PPCAssembler::lfsu(RegFPR frd, RegGPR ra, U16 d) { emitFormD(0xC4000000, frd, ra, d); }
void PPCAssembler::lfsux(RegFPR frd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00046E, frd, ra, rb); }
void PPCAssembler::lfsx(RegFPR frd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00042E, frd, ra, rb); }
void PPCAssembler::lha(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0xA8000000, rd, ra, d); }
void PPCAssembler::lhau(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0xAC000000, rd, ra, d); }
void PPCAssembler::lhaux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0002EE, rd, ra, rb); }
void PPCAssembler::lhax(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0002AE, rd, ra, rb); }
void PPCAssembler::lhbrx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00062C, rd, ra, rb); }
void PPCAssembler::lhz(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0xA0000000, rd, ra, d); }
void PPCAssembler::lhzu(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0xA4000000, rd, ra, d); }
void PPCAssembler::lhzux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00026E, rd, ra, rb); }
void PPCAssembler::lhzx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00022E, rd, ra, rb); }
void PPCAssembler::li(RegGPR rd, S16 simm) { addi(rd, r0, simm); }
void PPCAssembler::lis(RegGPR rd, S16 simm) { addis(rd, r0, simm); }
void PPCAssembler::lmw(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0xB8000000, rd, ra, d); }
void PPCAssembler::lswi() {  }
void PPCAssembler::lswx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00042A, rd, ra, rb); }
void PPCAssembler::lwa(RegGPR rd, RegGPR ra, U16 ds) { emitFormDS(0xE8000002, rd, ra, ds); }
void PPCAssembler::lwarx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C000028, rd, ra, rb); }
void PPCAssembler::lwaux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0002EA, rd, ra, rb); }
void PPCAssembler::lwax(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C0002AA, rd, ra, rb); }
void PPCAssembler::lwbrx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00042C, rd, ra, rb); }
void PPCAssembler::lwz(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0x80000000, rd, ra, d); }
void PPCAssembler::lwzu(RegGPR rd, RegGPR ra, U16 d) { emitFormD(0x84000000, rd, ra, d); }
void PPCAssembler::lwzux(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00006E, rd, ra, rb); }
void PPCAssembler::lwzx(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormX(0x7C00002E, rd, ra, rb); }
void PPCAssembler::mcrf(RegCR crfd, RegCR crfs) {  }
void PPCAssembler::mcrfs(RegCR crfd, RegCR crfs) {  }
void PPCAssembler::mffs() {  }
void PPCAssembler::mffs_() {  }
void PPCAssembler::mfocrf(RegGPR rd, U8 crm) { emitFormXFX(0x7C000026, rd, (0x200 | (crm << 1))); }
void PPCAssembler::mfcr(RegGPR rd) { emitFormXFX(0x7C000026, rd, 0); }
void PPCAssembler::mfspr(RegGPR rd, U16 spr) { emitFormXFX(0x7C0002A6, rd, ((spr & 0x1F) << 5) | ((spr >> 5) & 0x1F)); }
void PPCAssembler::mfxer(RegGPR rd) { mfspr(rd, 1); }
void PPCAssembler::mflr(RegGPR rd) { mfspr(rd, 8); }
void PPCAssembler::mfctr(RegGPR rd) { mfspr(rd, 9); }
void PPCAssembler::mftb(RegGPR rd, U16 tbr) { emitFormXFX(0x7C0002E6, rd, tbr); }
void PPCAssembler::mr(RegGPR ra, RegGPR rs) { or_(ra, rs, rs); }
void PPCAssembler::mtfsb0x() {  }
void PPCAssembler::mtfsb1x() {  }
void PPCAssembler::mtfsfix() {  }
void PPCAssembler::mtfsfx() {  }
void PPCAssembler::mtocrf(U8 crm, RegGPR rs) { emitFormXFX(0x7C000120, rs, (0x200 | (crm << 1))); }
void PPCAssembler::mtcrf(U8 crm, RegGPR rs) { emitFormXFX(0x7C000120, rs, (crm << 1)); }
void PPCAssembler::mtcr(RegGPR rs) { mtcrf(0xFF, rs); }
void PPCAssembler::mtspr(U16 spr, RegGPR rs) { emitFormXFX(0x7C0003A6, rs, ((spr & 0x1F) << 5) | ((spr >> 5) & 0x1F)); }
void PPCAssembler::mtxer(RegGPR rs) { mtspr(1, rs); }
void PPCAssembler::mtlr(RegGPR rs) { mtspr(8, rs); }
void PPCAssembler::mtctr(RegGPR rs) { mtspr(9, rs); }
void PPCAssembler::mulhd(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000092, rd, ra, rb); }
void PPCAssembler::mulhd_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000093, rd, ra, rb); }
void PPCAssembler::mulhdu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000012, rd, ra, rb); }
void PPCAssembler::mulhdu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000013, rd, ra, rb); }
void PPCAssembler::mulhw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000096, rd, ra, rb); }
void PPCAssembler::mulhw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000097, rd, ra, rb); }
void PPCAssembler::mulhwu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000016, rd, ra, rb); }
void PPCAssembler::mulhwu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000017, rd, ra, rb); }
void PPCAssembler::mulld(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D2, rd, ra, rb); }
void PPCAssembler::mulld_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D3, rd, ra, rb); }
void PPCAssembler::mulldo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D2, rd, ra, rb); }
void PPCAssembler::mulldo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D3, rd, ra, rb); }
void PPCAssembler::mulli(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x1C000000, rd, ra, simm); }
void PPCAssembler::mullw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D6, rd, ra, rb); }
void PPCAssembler::mullw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D7, rd, ra, rb); }
void PPCAssembler::mullwo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D6, rd, ra, rb); }
void PPCAssembler::mullwo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D7, rd, ra, rb); }
void PPCAssembler::nand(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C0003B8, rs, ra, rb); }
void PPCAssembler::nand_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C0003B9, rs, ra, rb); }
void PPCAssembler::neg(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0000D0, rd, ra, 0); }
void PPCAssembler::neg_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0000D1, rd, ra, 0); }
void PPCAssembler::nego(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0004D0, rd, ra, 0); }
void PPCAssembler::nego_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0004D1, rd, ra, 0); }
void PPCAssembler::nop() { ori(r0, r0, 0); }
void PPCAssembler::nor(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C0000F8, rs, ra, rb); }
void PPCAssembler::nor_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C0000F9, rs, ra, rb); }
void PPCAssembler::not_(RegGPR ra, RegGPR rs ) { nor(ra, rs, rs); }
void PPCAssembler::or_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000378, rs, ra, rb); }
void PPCAssembler::or__(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000379, rs, ra, rb); }
void PPCAssembler::orc(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000338, rs, ra, rb); }
void PPCAssembler::orc_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000339, rs, ra, rb); }
void PPCAssembler::ori(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x60000000, rs, ra, uimm); }
void PPCAssembler::oris(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x64000000, rs, ra, uimm); }
void PPCAssembler::rldcl(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb) {  }
void PPCAssembler::rldcl_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb) {  }
void PPCAssembler::rldcr(RegGPR ra, RegGPR rs, RegGPR rb, U8 me) {  }
void PPCAssembler::rldcr_(RegGPR ra, RegGPR rs, RegGPR rb, U8 me) {  }
void PPCAssembler::rldic(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldic_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicl(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicl_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicr(RegGPR ra, RegGPR rs, U8 sh, U8 me) {  }
void PPCAssembler::rldicr_(RegGPR ra, RegGPR rs, U8 sh, U8 me) {  }
void PPCAssembler::rldimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rlwimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwinm(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwinm_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwnm(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me) {  }
void PPCAssembler::rlwnm_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me) {  }
void PPCAssembler::sc() {  }
void PPCAssembler::sld(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000036, rs, ra, rb); }
void PPCAssembler::sld_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000037, rs, ra, rb); }
void PPCAssembler::slw(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000030, rs, ra, rb); }
void PPCAssembler::slw_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000031, rs, ra, rb); }
void PPCAssembler::srad(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000634, rs, ra, rb); }
void PPCAssembler::srad_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000635, rs, ra, rb); }
void PPCAssembler::sradi(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::sradi_(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::sraw(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000630, rs, ra, rb); }
void PPCAssembler::sraw_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000631, rs, ra, rb); }
void PPCAssembler::srawi(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::srawi_(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::srd(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srd_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srw(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srw_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::stb(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0x98000000, rs, ra, d); }
void PPCAssembler::stbu(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0x9C000000, rs, ra, d); }
void PPCAssembler::stbux(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0001EE, rs, ra, rb); }
void PPCAssembler::stbx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0001AE, rs, ra, rb); }
void PPCAssembler::std(RegGPR rs, RegGPR ra, U16 ds) { emitFormDS(0xF8000000, rs, ra, ds); }
void PPCAssembler::stdcx_(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0001AD, rs, ra, rb); }
void PPCAssembler::stdu(RegGPR rs, RegGPR ra, U16 ds) { emitFormDS(0xF8000001, rs, ra, ds); }
void PPCAssembler::stdux(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00016A, rs, ra, rb); }
void PPCAssembler::stdx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00012A, rs, ra, rb); }
void PPCAssembler::stfd(RegFPR frs, RegGPR ra, U16 d) { emitFormD(0xD8000000, frs, ra, d); }
void PPCAssembler::stfdu(RegFPR frs, RegGPR ra, U16 d) { emitFormD(0xDC000000, frs, ra, d); }
void PPCAssembler::stfdux(RegFPR frs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0005EE, frs, ra, rb); }
void PPCAssembler::stfdx(RegFPR frs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0005AE, frs, ra, rb); }
void PPCAssembler::stfiwx(RegFPR frs, RegGPR ra, RegGPR rb) { emitFormX(0x7C0007AE, frs, ra, rb); }
void PPCAssembler::stfs(RegFPR frs, RegGPR ra, U16 d) { emitFormD(0xD0000000, frs, ra, d); }
void PPCAssembler::stfsu(RegFPR frs, RegGPR ra, U16 d) { emitFormD(0xD4000000, frs, ra, d); }
void PPCAssembler::stfsux(RegFPR frs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00056E, frs, ra, rb); }
void PPCAssembler::stfsx(RegFPR frs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00052E, frs, ra, rb); }
void PPCAssembler::sth(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0xB0000000, rs, ra, d); }
void PPCAssembler::sthbrx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00072C, rs, ra, rb); }
void PPCAssembler::sthu(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0xB4000000, rs, ra, d); }
void PPCAssembler::sthux(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00036E, rs, ra, rb); }
void PPCAssembler::sthx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00032E, rs, ra, rb); }
void PPCAssembler::stmw(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0xBC000000, rs, ra, d); }
void PPCAssembler::stswi() {  }
void PPCAssembler::stswx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00052A, rs, ra, rb); }
void PPCAssembler::stw(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0x90000000, rs, ra, d); }
void PPCAssembler::stwbrx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00052C, rs, ra, rb); }
void PPCAssembler::stwcx_(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00012D, rs, ra, rb); }
void PPCAssembler::stwu(RegGPR rs, RegGPR ra, U16 d) { emitFormD(0x94000000, rs, ra, d); }
void PPCAssembler::stwux(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00016E, rs, ra, rb); }
void PPCAssembler::stwx(RegGPR rs, RegGPR ra, RegGPR rb) { emitFormX(0x7C00012E, rs, ra, rb); }
void PPCAssembler::subf(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000050, rd, ra, rb); }
void PPCAssembler::subf_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000051, rd, ra, rb); }
void PPCAssembler::subfc(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000010, rd, ra, rb); }
void PPCAssembler::subfc_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000011, rd, ra, rb); }
void PPCAssembler::subfco(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000410, rd, ra, rb); }
void PPCAssembler::subfco_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000411, rd, ra, rb); }
void PPCAssembler::subfe(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000110, rd, ra, rb); }
void PPCAssembler::subfe_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000111, rd, ra, rb); }
void PPCAssembler::subfeo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000510, rd, ra, rb); }
void PPCAssembler::subfeo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000511, rd, ra, rb); }
void PPCAssembler::subfic(RegGPR rd, RegGPR ra, S16 simm) { emitFormD(0x20000000, rd, ra, simm); }
void PPCAssembler::subfme(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D0, rd, ra, 0); }
void PPCAssembler::subfme_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D1, rd, ra, 0); }
void PPCAssembler::subfmeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D0, rd, ra, 0); }
void PPCAssembler::subfmeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D1, rd, ra, 0); }
void PPCAssembler::subfo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000450, rd, ra, rb); }
void PPCAssembler::subfo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000451, rd, ra, rb); }
void PPCAssembler::subfze(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000190, rd, ra, 0); }
void PPCAssembler::subfze_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000191, rd, ra, 0); }
void PPCAssembler::subfzeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000590, rd, ra, 0); }
void PPCAssembler::subfzeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000591, rd, ra, 0); }
void PPCAssembler::sync() {  }
void PPCAssembler::td() {  }
void PPCAssembler::tdi() {  }
void PPCAssembler::tw() {  }
void PPCAssembler::twi() {  }
void PPCAssembler::xor_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000278, rs, ra, rb); }
void PPCAssembler::xor__(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000279, rs, ra, rb); }
void PPCAssembler::xori(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x68000000, rs, ra, uimm); }
void PPCAssembler::xoris(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x6C000000, rs, ra, uimm); }

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
