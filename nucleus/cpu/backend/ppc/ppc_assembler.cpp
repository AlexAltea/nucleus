/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppc_assembler.h"
#include "nucleus/assert.h"

#include <cstdint>

namespace cpu {
namespace backend {
namespace ppc {

PPCAssembler::PPCAssembler(size_t codeSize, void* codeAddr) : Assembler(codeSize, codeAddr) {
}

// Emit instruction form
void PPCAssembler::emitFormI(U32 instruction, Operand li) {
    assert_true(li % 4 == 0, "The destination cannot overwrite the 2 LSb of a I-Form instruction");
    emit32(instruction | li);
}
void PPCAssembler::emitFormB(U32 instruction, Operand bo, Operand bi, Operand bd) {
    assert_true(bd % 4 == 0, "The destination cannot overwrite the 2 LSb of a B-Form instruction");
    const U32 boMask = (bo & 0x1F) << 21;
    const U32 biMask = (bi & 0x1F) << 16;
    emit32(instruction | boMask | biMask | bd);
}
void PPCAssembler::emitFormD(U32 instruction, Operand d, Operand a, U16 imm) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    emit32(instruction | rdMask | raMask | imm);
}
void PPCAssembler::emitFormDS(U32 instruction, Operand d, Operand a, U16 imm) {
    assert_true(imm % 4 == 0, "The immediate cannot overwrite the 2 LSb of a DS-Form instruction");
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    emit32(instruction | rdMask | raMask | imm);
}
void PPCAssembler::emitFormX(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit32(instruction | rdMask | raMask | rbMask);
}
void PPCAssembler::emitFormXL(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit32(instruction | rdMask | raMask | rbMask);
}
void PPCAssembler::emitFormXFX(U32 instruction, Operand d, Operand spr) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 sprMask = (spr & 0x3FF) << 11;
    emit32(instruction | rdMask | sprMask);
}
void PPCAssembler::emitFormXO(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rdMask = (d & 0x1F) << 21;
    const U32 raMask = (a & 0x1F) << 16;
    const U32 rbMask = (b & 0x1F) << 11;
    emit32(instruction | rdMask | raMask | rbMask);
}
void PPCAssembler::emitFormA(U32 instruction, Operand d, Operand a, Operand b, Operand c) {
    const U32 dMask = (d & 0x1F) << 21;
    const U32 aMask = (a & 0x1F) << 16;
    const U32 bMask = (b & 0x1F) << 11;
    const U32 cMask = (c & 0x1F) <<  6;
    emit32(instruction | dMask | aMask | bMask | cMask);
}
void PPCAssembler::emitFormM(U32 instruction, Operand s, Operand a, Operand b, Operand mb, Operand me) {
    const U32 sMask = (s & 0x1F) << 21;
    const U32 aMask = (a & 0x1F) << 16;
    const U32 bMask = (b & 0x1F) << 11;
    const U32 mbMask = (mb & 0x1F) << 6;
    const U32 meMask = (me & 0x1F) << 1;
    emit32(instruction | sMask | aMask | bMask | mbMask | meMask);
}

void PPCAssembler::emitFormVA(U32 instruction, Operand vd, Operand va, Operand vb, Operand vc) {
    const U32 vdMask = (vd & 0x1F) << 21;
    const U32 vaMask = (va & 0x1F) << 16;
    const U32 vbMask = (vb & 0x1F) << 11;
    const U32 vcMask = (vc & 0x1F) <<  6;
    emit32(instruction | vdMask | vaMask | vbMask | vcMask);
}

void PPCAssembler::emitFormVX(U32 instruction, Operand vd, Operand va, Operand vb) {
    const U32 vdMask = (vd & 0x1F) << 21;
    const U32 vaMask = (va & 0x1F) << 16;
    const U32 vbMask = (vb & 0x1F) << 11;
    emit32(instruction | vdMask | vaMask | vbMask);
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
void PPCAssembler::bc(U08 bo, U08 bi, U16 bd) { emitFormB(0x40000000, bo, bi, bd); }
void PPCAssembler::bc(U08 bo, U08 bi, Label label) { emitFormB(0x40000000, bo, bi, 0/*TODO*/); }
void PPCAssembler::blt(RegCR cr, Label label) { bc(12, (4 * cr) + 0, label); }
void PPCAssembler::ble(RegCR cr, Label label) { bc( 4, (4 * cr) + 1, label); }
void PPCAssembler::beq(RegCR cr, Label label) { bc(12, (4 * cr) + 2, label); }
void PPCAssembler::bge(RegCR cr, Label label) { bc( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgt(RegCR cr, Label label) { bc(12, (4 * cr) + 1, label); }
void PPCAssembler::bne(RegCR cr, Label label) { bc( 4, (4 * cr) + 2, label); }
void PPCAssembler::bso(RegCR cr, Label label) { bc(12, (4 * cr) + 3, label); }
void PPCAssembler::bca(U08 bo, U08 bi, U16 bd) { emitFormB(0x40000002, bo, bi, bd); }
void PPCAssembler::bca(U08 bo, U08 bi, Label label) { emitFormB(0x40000002, bo, bi, 0/*TODO*/); }
void PPCAssembler::blta(RegCR cr, Label label) { bca(12, (4 * cr) + 0, label); }
void PPCAssembler::blea(RegCR cr, Label label) { bca( 4, (4 * cr) + 1, label); }
void PPCAssembler::beqa(RegCR cr, Label label) { bca(12, (4 * cr) + 2, label); }
void PPCAssembler::bgea(RegCR cr, Label label) { bca( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgta(RegCR cr, Label label) { bca(12, (4 * cr) + 1, label); }
void PPCAssembler::bnea(RegCR cr, Label label) { bca( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsoa(RegCR cr, Label label) { bca(12, (4 * cr) + 3, label); }
void PPCAssembler::bcl(U08 bo, U08 bi, U16 bd) { emitFormB(0x40000001, bo, bi, bd); }
void PPCAssembler::bcl(U08 bo, U08 bi, Label label) { emitFormB(0x40000001, bo, bi, 0/*TODO*/); }
void PPCAssembler::bltl(RegCR cr, Label label) { bcl(12, (4 * cr) + 0, label); }
void PPCAssembler::blel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 1, label); }
void PPCAssembler::beql(RegCR cr, Label label) { bcl(12, (4 * cr) + 2, label); }
void PPCAssembler::bgel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgtl(RegCR cr, Label label) { bcl(12, (4 * cr) + 1, label); }
void PPCAssembler::bnel(RegCR cr, Label label) { bcl( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsol(RegCR cr, Label label) { bcl(12, (4 * cr) + 3, label); }
void PPCAssembler::bcla(U08 bo, U08 bi, U16 bd) { emitFormB(0x40000003, bo, bi, bd); }
void PPCAssembler::bcla(U08 bo, U08 bi, Label label) { emitFormB(0x40000003, bo, bi, 0/*TODO*/); }
void PPCAssembler::bltla(RegCR cr, Label label) { bcla(12, (4 * cr) + 0, label); }
void PPCAssembler::blela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 1, label); }
void PPCAssembler::beqla(RegCR cr, Label label) { bcla(12, (4 * cr) + 2, label); }
void PPCAssembler::bgela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 0, label); }
void PPCAssembler::bgtla(RegCR cr, Label label) { bcla(12, (4 * cr) + 1, label); }
void PPCAssembler::bnela(RegCR cr, Label label) { bcla( 4, (4 * cr) + 2, label); }
void PPCAssembler::bsola(RegCR cr, Label label) { bcla(12, (4 * cr) + 3, label); }
void PPCAssembler::bcctr(U08 bo, U08 bi, U08 bh, Label label) { emitFormXL(0x4C000420, bo, bi, bh); }
void PPCAssembler::bcctrl(U08 bo, U08 bi, U08 bh, Label label) { emitFormXL(0x4C000421, bo, bi, bh); }
void PPCAssembler::bclr(U08 bo, U08 bi, U08 bh, Label label) { emitFormXL(0x4C000020, bo, bi, bh); }
void PPCAssembler::bclrl(U08 bo, U08 bi, U08 bh, Label label) { emitFormXL(0x4C000021, bo, bi, bh); }
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
void PPCAssembler::crand(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000202, crfd, crfa, crfb); }
void PPCAssembler::crandc(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000102, crfd, crfa, crfb); }
void PPCAssembler::creqv(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000242, crfd, crfa, crfb); }
void PPCAssembler::crnand(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C0001C2, crfd, crfa, crfb); }
void PPCAssembler::crnor(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000042, crfd, crfa, crfb); }
void PPCAssembler::cror(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000382, crfd, crfa, crfb); }
void PPCAssembler::crorc(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000342, crfd, crfa, crfb); }
void PPCAssembler::crxor(U08 crfd, U08 crfa, U08 crfb) { emitFormXL(0x4C000182, crfd, crfa, crfb); }
void PPCAssembler::dcbf(RegGPR ra, RegGPR rb) { emitFormX(0x7C0000AC, 0, ra, rb); }
void PPCAssembler::dcbst(RegGPR ra, RegGPR rb) { emitFormX(0x7C00006C, 0, ra, rb); }
void PPCAssembler::dcbt(RegGPR ra, RegGPR rb, U08 th) { emitFormX(0x7C00022C, th, ra, rb); }
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
void PPCAssembler::fadd(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC00002A, frd, fra, frb, 0); }
void PPCAssembler::fadd_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC00002B, frd, fra, frb, 0); }
void PPCAssembler::fadds(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC00002A, frd, fra, frb, 0); }
void PPCAssembler::fadds_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC00002B, frd, fra, frb, 0); }
void PPCAssembler::fcfid(RegFPR frd, RegFPR frb) { emitFormX(0xFC00069C, frd, 0, frb); }
void PPCAssembler::fcfid_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00069D, frd, 0, frb); }
void PPCAssembler::fcmpo(RegCR crfd, RegFPR fra, RegFPR frb) { emitFormX(0xFC000040, (crfd << 2), fra, frb); }
void PPCAssembler::fcmpu(RegCR crfd, RegFPR fra, RegFPR frb) { emitFormX(0xFC000000, (crfd << 2), fra, frb); }
void PPCAssembler::fctid(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065C, frd, 0, frb); }
void PPCAssembler::fctid_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065D, frd, 0, frb); }
void PPCAssembler::fctidz(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065E, frd, 0, frb); }
void PPCAssembler::fctidz_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00065F, frd, 0, frb); }
void PPCAssembler::fctiw(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001C, frd, 0, frb); }
void PPCAssembler::fctiw_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001D, frd, 0, frb); }
void PPCAssembler::fctiwz(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001E, frd, 0, frb); }
void PPCAssembler::fctiwz_(RegFPR frd, RegFPR frb) { emitFormX(0xFC00001F, frd, 0, frb); }
void PPCAssembler::fdiv(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC000024, frd, fra, frb, 0); }
void PPCAssembler::fdiv_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC000025, frd, fra, frb, 0); }
void PPCAssembler::fdivs(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC000024, frd, fra, frb, 0); }
void PPCAssembler::fdivs_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC000025, frd, fra, frb, 0); }
void PPCAssembler::fmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003A, frd, fra, frb, frc); }
void PPCAssembler::fmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003B, frd, fra, frb, frc); }
void PPCAssembler::fmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003A, frd, fra, frb, frc); }
void PPCAssembler::fmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003B, frd, fra, frb, frc); }
void PPCAssembler::fmr(RegFPR frd, RegFPR frb) { emitFormX(0xFC000090, frd, 0, frb); }
void PPCAssembler::fmr_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000091, frd, 0, frb); }
void PPCAssembler::fmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC000038, frd, fra, frb, frc); }
void PPCAssembler::fmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC000039, frd, fra, frb, frc); }
void PPCAssembler::fmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC000038, frd, fra, frb, frc); }
void PPCAssembler::fmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC000039, frd, fra, frb, frc); }
void PPCAssembler::fmul(RegFPR frd, RegFPR fra, RegFPR frc) { emitFormA(0xFC000032, frd, fra, 0, frc); }
void PPCAssembler::fmul_(RegFPR frd, RegFPR fra, RegFPR frc) { emitFormA(0xFC000033, frd, fra, 0, frc); }
void PPCAssembler::fmuls(RegFPR frd, RegFPR fra, RegFPR frc) { emitFormA(0xEC000032, frd, fra, 0, frc); }
void PPCAssembler::fmuls_(RegFPR frd, RegFPR fra, RegFPR frc) { emitFormA(0xEC000033, frd, fra, 0, frc); }
void PPCAssembler::fnabs(RegFPR frd, RegFPR frb) { emitFormX(0xFC000110, frd, 0, frb); }
void PPCAssembler::fnabs_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000111, frd, 0, frb); }
void PPCAssembler::fneg(RegFPR frd, RegFPR frb) { emitFormX(0xFC000050, frd, 0, frb); }
void PPCAssembler::fneg_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000051, frd, 0, frb); }
void PPCAssembler::fnmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003E, frd, fra, frb, frc); }
void PPCAssembler::fnmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003F, frd, fra, frb, frc); }
void PPCAssembler::fnmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003E, frd, fra, frb, frc); }
void PPCAssembler::fnmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003F, frd, fra, frb, frc); }
void PPCAssembler::fnmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003C, frd, fra, frb, frc); }
void PPCAssembler::fnmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00003D, frd, fra, frb, frc); }
void PPCAssembler::fnmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003C, frd, fra, frb, frc); }
void PPCAssembler::fnmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xEC00003D, frd, fra, frb, frc); }
void PPCAssembler::fres(RegFPR frd, RegFPR frb) { emitFormA(0xEC000030, frd, 0, frb, 0); }
void PPCAssembler::fres_(RegFPR frd, RegFPR frb) { emitFormA(0xEC000031, frd, 0, frb, 0); }
void PPCAssembler::frsp(RegFPR frd, RegFPR frb) { emitFormX(0xFC000018, frd, 0, frb); }
void PPCAssembler::frsp_(RegFPR frd, RegFPR frb) { emitFormX(0xFC000019, frd, 0, frb); }
void PPCAssembler::frsqrte(RegFPR frd, RegFPR frb) { emitFormA(0xFC000034, frd, 0, frb, 0); }
void PPCAssembler::frsqrte_(RegFPR frd, RegFPR frb) { emitFormA(0xFC000035, frd, 0, frb, 0); }
void PPCAssembler::fsel(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00002E, frd, fra, frb, frc); }
void PPCAssembler::fsel_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb) { emitFormA(0xFC00002F, frd, fra, frb, frc); }
void PPCAssembler::fsqrt(RegFPR frd, RegFPR frb) { emitFormA(0xFC00002C, frd, 0, frb, 0); }
void PPCAssembler::fsqrt_(RegFPR frd, RegFPR frb) { emitFormA(0xFC00002D, frd, 0, frb, 0); }
void PPCAssembler::fsqrts(RegFPR frd, RegFPR frb) { emitFormA(0xEC00002C, frd, 0, frb, 0); }
void PPCAssembler::fsqrts_(RegFPR frd, RegFPR frb) { emitFormA(0xEC00002D, frd, 0, frb, 0); }
void PPCAssembler::fsub(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC000028, frd, fra, frb, 0); }
void PPCAssembler::fsub_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xFC000029, frd, fra, frb, 0); }
void PPCAssembler::fsubs(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC000028, frd, fra, frb, 0); }
void PPCAssembler::fsubs_(RegFPR frd, RegFPR fra, RegFPR frb) { emitFormA(0xEC000029, frd, fra, frb, 0); }
void PPCAssembler::icbi(RegGPR ra, RegGPR rb) { emitFormX(0x7C0007AC, 0, ra, rb); }
void PPCAssembler::isync() { emitFormXL(0x4C00012C, 0, 0, 0); }
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
void PPCAssembler::mfocrf(RegGPR rd, U08 crm) { emitFormXFX(0x7C000026, rd, (0x200 | (crm << 1))); }
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
void PPCAssembler::mtocrf(U08 crm, RegGPR rs) { emitFormXFX(0x7C000120, rs, (0x200 | (crm << 1))); }
void PPCAssembler::mtcrf(U08 crm, RegGPR rs) { emitFormXFX(0x7C000120, rs, (crm << 1)); }
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
void PPCAssembler::rldcl(RegGPR ra, RegGPR rs, RegGPR rb, U08 mb) {  }
void PPCAssembler::rldcl_(RegGPR ra, RegGPR rs, RegGPR rb, U08 mb) {  }
void PPCAssembler::rldcr(RegGPR ra, RegGPR rs, RegGPR rb, U08 me) {  }
void PPCAssembler::rldcr_(RegGPR ra, RegGPR rs, RegGPR rb, U08 me) {  }
void PPCAssembler::rldic(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rldic_(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rldicl(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rldicl_(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rldicr(RegGPR ra, RegGPR rs, U08 sh, U08 me) {  }
void PPCAssembler::rldicr_(RegGPR ra, RegGPR rs, U08 sh, U08 me) {  }
void PPCAssembler::rldimi(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rldimi_(RegGPR ra, RegGPR rs, U08 sh, U08 mb) {  }
void PPCAssembler::rlwimi(RegGPR ra, RegGPR rs, U08 sh, U08 mb, U08 me) { emitFormM(0x50000000, rs, ra, sh, mb, me); }
void PPCAssembler::rlwimi_(RegGPR ra, RegGPR rs, U08 sh, U08 mb, U08 me) { emitFormM(0x50000001, rs, ra, sh, mb, me); }
void PPCAssembler::rlwinm(RegGPR ra, RegGPR rs, U08 sh, U08 mb, U08 me) { emitFormM(0x54000000, rs, ra, sh, mb, me); }
void PPCAssembler::rlwinm_(RegGPR ra, RegGPR rs, U08 sh, U08 mb, U08 me) { emitFormM(0x54000001, rs, ra, sh, mb, me); }
void PPCAssembler::rlwnm(RegGPR ra, RegGPR rs, RegGPR rb, U08 mb, U08 me) { emitFormM(0x5C000000, rs, ra, rb, mb, me); }
void PPCAssembler::rlwnm_(RegGPR ra, RegGPR rs, RegGPR rb, U08 mb, U08 me) { emitFormM(0x5C000001, rs, ra, rb, mb, me); }
void PPCAssembler::sc() {  }
void PPCAssembler::sld(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000036, rs, ra, rb); }
void PPCAssembler::sld_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000037, rs, ra, rb); }
void PPCAssembler::slw(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000030, rs, ra, rb); }
void PPCAssembler::slw_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000031, rs, ra, rb); }
void PPCAssembler::srad(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000634, rs, ra, rb); }
void PPCAssembler::srad_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000635, rs, ra, rb); }
void PPCAssembler::sradi(RegGPR ra, RegGPR rs, U08 sh) {  }
void PPCAssembler::sradi_(RegGPR ra, RegGPR rs, U08 sh) {  }
void PPCAssembler::sraw(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000630, rs, ra, rb); }
void PPCAssembler::sraw_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000631, rs, ra, rb); }
void PPCAssembler::srawi(RegGPR ra, RegGPR rs, U08 sh) { emitFormX(0x7C000670, rs, ra, sh); }
void PPCAssembler::srawi_(RegGPR ra, RegGPR rs, U08 sh) { emitFormX(0x7C000671, rs, ra, sh); }
void PPCAssembler::srd(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000436, rs, ra, rb); }
void PPCAssembler::srd_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000437, rs, ra, rb); }
void PPCAssembler::srw(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000430, rs, ra, rb); }
void PPCAssembler::srw_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000431, rs, ra, rb); }
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
void PPCAssembler::sync(U08 l) { emitFormX(0x7C0004AC, l, 0, 0); }
void PPCAssembler::td() {  }
void PPCAssembler::tdi() {  }
void PPCAssembler::tw() {  }
void PPCAssembler::twi() {  }
void PPCAssembler::xor_(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000278, rs, ra, rb); }
void PPCAssembler::xor__(RegGPR ra, RegGPR rs, RegGPR rb) { emitFormX(0x7C000279, rs, ra, rb); }
void PPCAssembler::xori(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x68000000, rs, ra, uimm); }
void PPCAssembler::xoris(RegGPR ra, RegGPR rs, U16 uimm) { emitFormD(0x6C000000, rs, ra, uimm); }

// PPC64 Vector/SIMD Instructions (aka AltiVec)
void PPCAssembler::lvebx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00000E, vd, va, vb); }
void PPCAssembler::lvehx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00004E, vd, va, vb); }
void PPCAssembler::lvewx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00008E, vd, va, vb); }
void PPCAssembler::lvlx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00040E, vd, va, vb); }
void PPCAssembler::lvlxl(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00060E, vd, va, vb); }
void PPCAssembler::lvrx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00044E, vd, va, vb); }
void PPCAssembler::lvrxl(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00064E, vd, va, vb); }
void PPCAssembler::lvsl(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00000C, vd, va, vb); }
void PPCAssembler::lvsr(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C00004C, vd, va, vb); }
void PPCAssembler::lvx(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C0000CE, vd, va, vb); }
void PPCAssembler::lvxl(RegVR vd, RegVR va, RegVR vb) { emitFormX(0x7C0002CE, vd, va, vb); }
void PPCAssembler::mfvscr(RegVR vd) { emitFormVX(0x10000604, vd, 0, 0); }
void PPCAssembler::mtvscr(RegVR vb) { emitFormVX(0x10000644, 0, 0, vb); }
void PPCAssembler::stvebx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00010E, vs, va, vb); }
void PPCAssembler::stvehx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00014E, vs, va, vb); }
void PPCAssembler::stvewx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00018E, vs, va, vb); }
void PPCAssembler::stvlx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00050E, vs, va, vb); }
void PPCAssembler::stvlxl(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00070E, vs, va, vb); }
void PPCAssembler::stvrx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00054E, vs, va, vb); }
void PPCAssembler::stvrxl(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C00074E, vs, va, vb); }
void PPCAssembler::stvx(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C0001CE, vs, va, vb); }
void PPCAssembler::stvxl(RegVR vs, RegVR va, RegVR vb) { emitFormX(0x7C0003CE, vs, va, vb); }
void PPCAssembler::vaddcuw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000180, vd, va, vb); }
void PPCAssembler::vaddfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000000A, vd, va, vb); }
void PPCAssembler::vaddsbs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000300, vd, va, vb); }
void PPCAssembler::vaddshs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000340, vd, va, vb); }
void PPCAssembler::vaddsws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000380, vd, va, vb); }
void PPCAssembler::vaddubm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000000, vd, va, vb); }
void PPCAssembler::vaddubs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000200, vd, va, vb); }
void PPCAssembler::vadduhm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000040, vd, va, vb); }
void PPCAssembler::vadduhs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000240, vd, va, vb); }
void PPCAssembler::vadduwm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000080, vd, va, vb); }
void PPCAssembler::vadduws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000280, vd, va, vb); }
void PPCAssembler::vand(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000404, vd, va, vb); }
void PPCAssembler::vandc(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000444, vd, va, vb); }
void PPCAssembler::vavgsb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000502, vd, va, vb); }
void PPCAssembler::vavgsh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000542, vd, va, vb); }
void PPCAssembler::vavgsw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000582, vd, va, vb); }
void PPCAssembler::vavgub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000402, vd, va, vb); }
void PPCAssembler::vavguh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000442, vd, va, vb); }
void PPCAssembler::vavguw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000482, vd, va, vb); }
void PPCAssembler::vcfsx(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000034A, vd, uimm, vb); }
void PPCAssembler::vcfux(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000030A, vd, uimm, vb); }
void PPCAssembler::vcmpbfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100003C6, vd, va, vb); }
void PPCAssembler::vcmpbfp_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100007C6, vd, va, vb); }
void PPCAssembler::vcmpeqfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100000C6, vd, va, vb); }
void PPCAssembler::vcmpeqfp_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100004C6, vd, va, vb); }
void PPCAssembler::vcmpequb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000006, vd, va, vb); }
void PPCAssembler::vcmpequb_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000406, vd, va, vb); }
void PPCAssembler::vcmpequh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000046, vd, va, vb); }
void PPCAssembler::vcmpequh_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000446, vd, va, vb); }
void PPCAssembler::vcmpequw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000086, vd, va, vb); }
void PPCAssembler::vcmpequw_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000486, vd, va, vb); }
void PPCAssembler::vcmpgefp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100001C6, vd, va, vb); }
void PPCAssembler::vcmpgefp_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100005C6, vd, va, vb); }
void PPCAssembler::vcmpgtfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100002C6, vd, va, vb); }
void PPCAssembler::vcmpgtfp_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100006C6, vd, va, vb); }
void PPCAssembler::vcmpgtsb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000306, vd, va, vb); }
void PPCAssembler::vcmpgtsb_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000706, vd, va, vb); }
void PPCAssembler::vcmpgtsh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000346, vd, va, vb); }
void PPCAssembler::vcmpgtsh_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000746, vd, va, vb); }
void PPCAssembler::vcmpgtsw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000386, vd, va, vb); }
void PPCAssembler::vcmpgtsw_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000786, vd, va, vb); }
void PPCAssembler::vcmpgtub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000206, vd, va, vb); }
void PPCAssembler::vcmpgtub_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000606, vd, va, vb); }
void PPCAssembler::vcmpgtuh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000246, vd, va, vb); }
void PPCAssembler::vcmpgtuh_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000646, vd, va, vb); }
void PPCAssembler::vcmpgtuw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000286, vd, va, vb); }
void PPCAssembler::vcmpgtuw_(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000686, vd, va, vb); }
void PPCAssembler::vctsxs(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x100003CA, vd, uimm, vb); }
void PPCAssembler::vctuxs(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000038A, vd, uimm, vb); }
void PPCAssembler::vexptefp(RegVR vd, RegVR vb) { emitFormVX(0x1000018A, vd, 0, vb); }
void PPCAssembler::vlogefp(RegVR vd, RegVR vb) { emitFormVX(0x100001CA, vd, 0, vb); }
void PPCAssembler::vmaddfp(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x1000002E, vd, va, vb, vc); }
void PPCAssembler::vmaxfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000040A, vd, va, vb); }
void PPCAssembler::vmaxsb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000102, vd, va, vb); }
void PPCAssembler::vmaxsh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000142, vd, va, vb); }
void PPCAssembler::vmaxsw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000182, vd, va, vb); }
void PPCAssembler::vmaxub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000002, vd, va, vb); }
void PPCAssembler::vmaxuh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000042, vd, va, vb); }
void PPCAssembler::vmaxuw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000082, vd, va, vb); }
void PPCAssembler::vmhaddshs(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000020, vd, va, vb, vc); }
void PPCAssembler::vmhraddshs(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000021, vd, va, vb, vc); }
void PPCAssembler::vminfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000044A, vd, va, vb); }
void PPCAssembler::vminsb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000302, vd, va, vb); }
void PPCAssembler::vminsh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000342, vd, va, vb); }
void PPCAssembler::vminsw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000382, vd, va, vb); }
void PPCAssembler::vminub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000202, vd, va, vb); }
void PPCAssembler::vminuh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000242, vd, va, vb); }
void PPCAssembler::vminuw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000282, vd, va, vb); }
void PPCAssembler::vmladduhm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000022, vd, va, vb, vc); }
void PPCAssembler::vmrghb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000000C, vd, va, vb); }
void PPCAssembler::vmrghh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000004C, vd, va, vb); }
void PPCAssembler::vmrghw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000008C, vd, va, vb); }
void PPCAssembler::vmrglb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000010C, vd, va, vb); }
void PPCAssembler::vmrglh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000014C, vd, va, vb); }
void PPCAssembler::vmrglw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000018C, vd, va, vb); }
void PPCAssembler::vmsummbm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000025, vd, va, vb, vc); }
void PPCAssembler::vmsumshm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000028, vd, va, vb, vc); }
void PPCAssembler::vmsumshs(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000029, vd, va, vb, vc); }
void PPCAssembler::vmsumubm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000024, vd, va, vb, vc); }
void PPCAssembler::vmsumuhm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000026, vd, va, vb, vc); }
void PPCAssembler::vmsumuhs(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x10000027, vd, va, vb, vc); }
void PPCAssembler::vmulesb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000308, vd, va, vb); }
void PPCAssembler::vmulesh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000348, vd, va, vb); }
void PPCAssembler::vmuleub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000208, vd, va, vb); }
void PPCAssembler::vmuleuh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000248, vd, va, vb); }
void PPCAssembler::vmulosb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000108, vd, va, vb); }
void PPCAssembler::vmulosh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000148, vd, va, vb); }
void PPCAssembler::vmuloub(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000008, vd, va, vb); }
void PPCAssembler::vmulouh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000048, vd, va, vb); }
void PPCAssembler::vnmsubfp(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x1000002F, vd, va, vb, vc); }
void PPCAssembler::vnor(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000504, vd, va, vb); }
void PPCAssembler::vor(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000484, vd, va, vb); }
void PPCAssembler::vperm(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x1000002B, vd, va, vb, vc); }
void PPCAssembler::vpkpx(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000030E, vd, va, vb); }
void PPCAssembler::vpkshss(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000018E, vd, va, vb); }
void PPCAssembler::vpkshus(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000010E, vd, va, vb); }
void PPCAssembler::vpkswss(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100001CE, vd, va, vb); }
void PPCAssembler::vpkswus(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000014E, vd, va, vb); }
void PPCAssembler::vpkuhum(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000000E, vd, va, vb); }
void PPCAssembler::vpkuhus(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000008E, vd, va, vb); }
void PPCAssembler::vpkuwum(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000004E, vd, va, vb); }
void PPCAssembler::vpkuwus(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100000CE, vd, va, vb); }
void PPCAssembler::vrefp(RegVR vd, RegVR vb) { emitFormVX(0x1000010A, vd, 0, vb); }
void PPCAssembler::vrfim(RegVR vd, RegVR vb) { emitFormVX(0x100002CA, vd, 0, vb); }
void PPCAssembler::vrfin(RegVR vd, RegVR vb) { emitFormVX(0x1000020A, vd, 0, vb); }
void PPCAssembler::vrfip(RegVR vd, RegVR vb) { emitFormVX(0x1000028A, vd, 0, vb); }
void PPCAssembler::vrfiz(RegVR vd, RegVR vb) { emitFormVX(0x1000024A, vd, 0, vb); }
void PPCAssembler::vrlb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000004, vd, va, vb); }
void PPCAssembler::vrlh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000044, vd, va, vb); }
void PPCAssembler::vrlw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000084, vd, va, vb); }
void PPCAssembler::vrsqrtefp(RegVR vd, RegVR vb) { emitFormVX(0x1000014A, vd, 0, vb); }
void PPCAssembler::vsel(RegVR vd, RegVR va, RegVR vb, RegVR vc) { emitFormVA(0x1000002A, vd, va, vb, vc); }
void PPCAssembler::vsl(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100001C4, vd, va, vb); }
void PPCAssembler::vslb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000104, vd, va, vb); }
void PPCAssembler::vsldoi(RegVR vd, RegVR va, RegVR vb, U08 shb) { emitFormVA(0x1000002C, vd, va, vb, shb & 0xF); }
void PPCAssembler::vslh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000144, vd, va, vb); }
void PPCAssembler::vslo(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000040C, vd, va, vb); }
void PPCAssembler::vslw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000184, vd, va, vb); }
void PPCAssembler::vspltb(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000020C, vd, uimm, vb); }
void PPCAssembler::vsplth(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000024C, vd, uimm, vb); }
void PPCAssembler::vspltisb(RegVR vd, RegVR vb, S08 simm) { emitFormVX(0x1000030C, vd, simm, vb); }
void PPCAssembler::vspltish(RegVR vd, S08 simm) { emitFormVX(0x1000034C, vd, simm, 0); }
void PPCAssembler::vspltisw(RegVR vd, S08 simm) { emitFormVX(0x1000038C, vd, simm, 0); }
void PPCAssembler::vspltw(RegVR vd, RegVR vb, U08 uimm) { emitFormVX(0x1000028C, vd, uimm, vb); }
void PPCAssembler::vsr(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100002C4, vd, va, vb); }
void PPCAssembler::vsrab(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000304, vd, va, vb); }
void PPCAssembler::vsrah(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000344, vd, va, vb); }
void PPCAssembler::vsraw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000384, vd, va, vb); }
void PPCAssembler::vsrb(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000204, vd, va, vb); }
void PPCAssembler::vsrh(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000244, vd, va, vb); }
void PPCAssembler::vsro(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000044C, vd, va, vb); }
void PPCAssembler::vsrw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000284, vd, va, vb); }
void PPCAssembler::vsubcuw(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000580, vd, va, vb); }
void PPCAssembler::vsubfp(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x1000004A, vd, va, vb); }
void PPCAssembler::vsubsbs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000700, vd, va, vb); }
void PPCAssembler::vsubshs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000740, vd, va, vb); }
void PPCAssembler::vsubsws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000780, vd, va, vb); }
void PPCAssembler::vsububm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000400, vd, va, vb); }
void PPCAssembler::vsububs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000600, vd, va, vb); }
void PPCAssembler::vsubuhm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000440, vd, va, vb); }
void PPCAssembler::vsubuhs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000640, vd, va, vb); }
void PPCAssembler::vsubuwm(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000480, vd, va, vb); }
void PPCAssembler::vsubuws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000680, vd, va, vb); }
void PPCAssembler::vsum2sws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000688, vd, va, vb); }
void PPCAssembler::vsum4sbs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000708, vd, va, vb); }
void PPCAssembler::vsum4shs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000648, vd, va, vb); }
void PPCAssembler::vsum4ubs(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000608, vd, va, vb); }
void PPCAssembler::vsumsws(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x10000788, vd, va, vb); }
void PPCAssembler::vupkhpx(RegVR vd, RegVR vb) { emitFormVX(0x1000034E, vd, 0, vb); }
void PPCAssembler::vupkhsb(RegVR vd, RegVR vb) { emitFormVX(0x1000020E, vd, 0, vb); }
void PPCAssembler::vupkhsh(RegVR vd, RegVR vb) { emitFormVX(0x1000024E, vd, 0, vb); }
void PPCAssembler::vupklpx(RegVR vd, RegVR vb) { emitFormVX(0x100003CE, vd, 0, vb); }
void PPCAssembler::vupklsb(RegVR vd, RegVR vb) { emitFormVX(0x1000028E, vd, 0, vb); }
void PPCAssembler::vupklsh(RegVR vd, RegVR vb) { emitFormVX(0x100002CE, vd, 0, vb); }
void PPCAssembler::vxor(RegVR vd, RegVR va, RegVR vb) { emitFormVX(0x100004C4, vd, va, vb); }

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
