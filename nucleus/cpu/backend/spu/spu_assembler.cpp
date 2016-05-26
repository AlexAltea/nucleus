/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_assembler.h"
#include "nucleus/assert.h"

#include <cstdint>

namespace cpu {
namespace backend {
namespace spu {

// Emit instruction form
void SPUAssembler::emitFormRR(U32 instruction, Operand rt, Operand ra, Operand rb) {
    const U32 rtMask = (rt & 0x7F);
    const U32 raMask = (ra & 0x7F) << 7;
    const U32 rbMask = (rb & 0x7F) << 14;
    emit32(instruction | rbMask | raMask | rtMask);
}
void SPUAssembler::emitFormRRR(U32 instruction, Operand rt, Operand ra, Operand rb, Operand rc) {
    const U32 rcMask = (rc & 0x7F);
    const U32 raMask = (ra & 0x7F) << 7;
    const U32 rbMask = (rb & 0x7F) << 14;
    const U32 rtMask = (rt & 0x7F) << 21;
    emit32(instruction | rtMask | raMask | rbMask | rcMask);
}
void SPUAssembler::emitFormRI7(U32 instruction, Operand rt, Operand ra, U32 i7) {
    assert_true((i7 & ~0x7F) == 0, "Immediate field should be 7-bits in size");
    const U32 rtMask = (rt & 0x7F);
    const U32 raMask = (ra & 0x7F) << 7;
    const U32 i7Mask = (i7 & 0x7F) << 14;
    emit32(instruction | i7Mask | raMask | rtMask);
}
void SPUAssembler::emitFormRI8(U32 instruction, Operand rt, Operand ra, U32 i8) {
    assert_true((i8 & ~0xFF) == 0, "Immediate field should be 8-bits in size");
    const U32 rtMask = (rt & 0x7F);
    const U32 raMask = (ra & 0x7F) << 7;
    const U32 i8Mask = (i8 & 0x7F) << 14;
    emit32(instruction | i8Mask | raMask | rtMask);
}
void SPUAssembler::emitFormRI10(U32 instruction, Operand rt, Operand ra, U32 i10) {
    assert_true((i10 & ~0x3FF) == 0, "Immediate field should be 10-bits in size");
    const U32 rtMask = (rt & 0x7F);
    const U32 raMask = (ra & 0x7F) << 7;
    const U32 i10Mask = (i10 & 0x3FF) << 14;
    emit32(instruction | i10Mask | raMask | rtMask);
}
void SPUAssembler::emitFormRI16(U32 instruction, Operand rt, U32 i16) {
    assert_true((i16 & ~0xFFFF) == 0, "Immediate field should be 16-bits in size");
    const U32 rtMask = (rt & 0x7F);
    const U32 i16Mask = (i16 & 0xFFFF) << 7;
    emit32(instruction | i16Mask | rtMask);
}
void SPUAssembler::emitFormRI18(U32 instruction, Operand rt, U32 i18) {
    assert_true((i18 & ~0x3FFFF) == 0, "Immediate field should be 18-bits in size");
    const U32 rtMask = (rt & 0x7F);
    const U32 i18Mask = (i18 & 0x3FFFF) << 7;
    emit32(instruction | i18Mask | rtMask);
}

// SPU instructions
void SPUAssembler::a(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x18000000, rt, ra, rb); }
void SPUAssembler::absdb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0A600000, rt, ra, rb); }
void SPUAssembler::addx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x68000000, rt, ra, rb); }
void SPUAssembler::ah(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x19000000, rt, ra, rb); }
void SPUAssembler::ahi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x1D000000, rt, ra, i10); }
void SPUAssembler::ai(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x1C000000, rt, ra, i10); }
void SPUAssembler::and_(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x18200000, rt, ra, rb); }
void SPUAssembler::andbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x16000000, rt, ra, i10); }
void SPUAssembler::andc(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58200000, rt, ra, rb); }
void SPUAssembler::andhi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x15000000, rt, ra, i10); }
void SPUAssembler::andi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x14000000, rt, ra, i10); }
void SPUAssembler::avgb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x1A600000, rt, ra, rb); }
void SPUAssembler::bg(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x08400000, rt, ra, rb); }
void SPUAssembler::bgx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x68600000, rt, ra, rb); }
void SPUAssembler::bi(RegGPR ra) { emitFormRR(0x35000000, 0, ra, 0); } // TODO
void SPUAssembler::bihnz(RegGPR rt, RegGPR ra) { emitFormRR(0x25600000, rt, ra, 0); }
void SPUAssembler::bihz(RegGPR rt, RegGPR ra) { emitFormRR(0x25400000, rt, ra, 0); }
void SPUAssembler::binz(RegGPR rt, RegGPR ra) { emitFormRR(0x25200000, rt, ra, 0); }
void SPUAssembler::bisl(RegGPR rt, RegGPR ra) { emitFormRR(0x35200000, rt, ra, 0); }
void SPUAssembler::bisled(RegGPR rt, RegGPR ra) { emitFormRR(0x35600000, rt, ra, 0); }
void SPUAssembler::biz(RegGPR rt, RegGPR ra) { emitFormRR(0x25000000, rt, ra, 0); }
void SPUAssembler::br(U32 i16) { emitFormRI16(0x32000000, 0, i16); }
void SPUAssembler::bra(U32 i16) { emitFormRI16(0x30000000, 0, i16); }
void SPUAssembler::brasl(RegGPR rt, U32 i16) { emitFormRI16(0x31000000, rt, i16); }
void SPUAssembler::brhnz(RegGPR rt, U32 i16) { emitFormRI16(0x23000000, rt, i16); }
void SPUAssembler::brhz(RegGPR rt, U32 i16) { emitFormRI16(0x22000000, rt, i16); }
void SPUAssembler::brnz(RegGPR rt, U32 i16) { emitFormRI16(0x21000000, rt, i16); }
void SPUAssembler::brsl(RegGPR rt, U32 i16) { emitFormRI16(0x33000000, rt, i16); }
void SPUAssembler::brz(RegGPR rt, U32 i16) { emitFormRI16(0x20000000, rt, i16); }
void SPUAssembler::cbd(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3E800000, rt, ra, i7); }
void SPUAssembler::cbx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3A800000, rt, ra, rb); }
void SPUAssembler::cdd(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3EE00000, rt, ra, i7); }
void SPUAssembler::cdx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3AE00000, rt, ra, rb); }
void SPUAssembler::ceq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78000000, rt, ra, rb); }
void SPUAssembler::ceqb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x7A000000, rt, ra, rb); }
void SPUAssembler::ceqbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x7E000000, rt, ra, i10); }
void SPUAssembler::ceqh(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x79000000, rt, ra, rb); }
void SPUAssembler::ceqhi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x7D000000, rt, ra, i10); }
void SPUAssembler::ceqi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x7C000000, rt, ra, i10); }
void SPUAssembler::cflts(RegGPR rt, RegGPR ra, U32 i8) { emitFormRI8(0x76000000, rt, ra, i8); }
void SPUAssembler::cfltu(RegGPR rt, RegGPR ra, U32 i8) { emitFormRI8(0x76400000, rt, ra, i8); }
void SPUAssembler::cg(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x18400000, rt, ra, rb); }
void SPUAssembler::cgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x48000000, rt, ra, rb); }
void SPUAssembler::cgtb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x4A000000, rt, ra, rb); }
void SPUAssembler::cgtbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x4E000000, rt, ra, i10); }
void SPUAssembler::cgth(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x49000000, rt, ra, rb); }
void SPUAssembler::cgthi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x4D000000, rt, ra, i10); }
void SPUAssembler::cgti(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x4C000000, rt, ra, i10); }
void SPUAssembler::cgx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x68400000, rt, ra, rb); }
void SPUAssembler::chd(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3EA00000, rt, ra, i7); }
void SPUAssembler::chx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3AA00000, rt, ra, rb); }
void SPUAssembler::clgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58000000, rt, ra, rb); }
void SPUAssembler::clgtb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x5A000000, rt, ra, rb); }
void SPUAssembler::clgtbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x5E000000, rt, ra, i10); }
void SPUAssembler::clgth(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59000000, rt, ra, rb); }
void SPUAssembler::clgthi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x5D000000, rt, ra, i10); }
void SPUAssembler::clgti(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x5C000000, rt, ra, i10); }
void SPUAssembler::clz(RegGPR rt, RegGPR ra) { emitFormRR(0x54A00000, rt, ra, 0); }
void SPUAssembler::cntb(RegGPR rt, RegGPR ra) { emitFormRR(0x56800000, rt, ra, 0); }
void SPUAssembler::csflt(RegGPR rt, RegGPR ra, U32 i8) { emitFormRI8(0x76800000, rt, ra, i8); }
void SPUAssembler::cuflt(RegGPR rt, RegGPR ra, U32 i8) { emitFormRI8(0x76C00000, rt, ra, i8); }
void SPUAssembler::cwd(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3EC00000, rt, ra, i7); }
void SPUAssembler::cwx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3AC00000, rt, ra, rb); }
void SPUAssembler::dfa(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59800000, rt, ra, rb); }
void SPUAssembler::dfceq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78600000, rt, ra, rb); }
void SPUAssembler::dfcgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58600000, rt, ra, rb); }
void SPUAssembler::dfcmeq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x79600000, rt, ra, rb); }
void SPUAssembler::dfcmgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59600000, rt, ra, rb); }
void SPUAssembler::dfm(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59C00000, rt, ra, rb); }
void SPUAssembler::dfma(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x6B800000, rt, ra, rb); }
void SPUAssembler::dfms(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x6BA00000, rt, ra, rb); }
void SPUAssembler::dfnma(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x6BE00000, rt, ra, rb); }
void SPUAssembler::dfnms(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x6BC00000, rt, ra, rb); }
void SPUAssembler::dfs(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59A00000, rt, ra, rb); }
void SPUAssembler::dftsv(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x77E00000, rt, ra, i7); }
void SPUAssembler::dsync() {emitFormRR(0x00600000, 0, 0, 0); }
void SPUAssembler::eqv(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x49200000, rt, ra, rb); }
void SPUAssembler::fa(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58800000, rt, ra, rb); }
void SPUAssembler::fceq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78400000, rt, ra, rb); }
void SPUAssembler::fcgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58400000, rt, ra, rb); }
void SPUAssembler::fcmeq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x79400000, rt, ra, rb); }
void SPUAssembler::fcmgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59400000, rt, ra, rb); }
void SPUAssembler::fesd(RegGPR rt, RegGPR ra) { emitFormRR(0x77000000, rt, ra, 0); }
void SPUAssembler::fi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x7A800000, rt, ra, rb); }
void SPUAssembler::fm(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58C00000, rt, ra, rb); }
void SPUAssembler::fma(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0xE0000000, rt, ra, rb, rc); }
void SPUAssembler::fms(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0xF0000000, rt, ra, rb, rc); }
void SPUAssembler::fnms(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0xD0000000, rt, ra, rb, rc); }
void SPUAssembler::frds(RegGPR rt, RegGPR ra) { emitFormRR(0x77200000, rt, ra, 0); }
void SPUAssembler::frest(RegGPR rt, RegGPR ra) { emitFormRR(0x37000000, rt, ra, 0); }
void SPUAssembler::frsqest(RegGPR rt, RegGPR ra) { emitFormRR(0x37200000, rt, ra, 0); }
void SPUAssembler::fs(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x58A00000, rt, ra, rb); }
void SPUAssembler::fscrrd(RegGPR rt) { emitFormRR(0x73000000, rt, 0, 0); }
void SPUAssembler::fscrwr(RegGPR rt, RegGPR ra) { emitFormRR(0x77400000, rt, ra, 0); }
void SPUAssembler::fsm(RegGPR rt, RegGPR ra) { emitFormRR(0x36800000, rt, ra, 0); }
void SPUAssembler::fsmb(RegGPR rt, RegGPR ra) { emitFormRR(0x36C00000, rt, ra, 0); }
void SPUAssembler::fsmbi(RegGPR rt, U32 i16) { emitFormRI16(0x32800000, rt, i16); }
void SPUAssembler::fsmh(RegGPR rt, RegGPR ra) { emitFormRR(0x36A00000, rt, ra, 0); }
void SPUAssembler::gb(RegGPR rt, RegGPR ra) { emitFormRR(0x36000000, rt, ra, 0); }
void SPUAssembler::gbb(RegGPR rt, RegGPR ra) { emitFormRR(0x36400000, rt, ra, 0); }
void SPUAssembler::gbh(RegGPR rt, RegGPR ra) { emitFormRR(0x36200000, rt, ra, 0); }
void SPUAssembler::hbr(U32 ro, U32 i7) { } // TODO: 0x35800000
void SPUAssembler::hbra(U32 ro, U32 i16) { } // TODO: 0x10000000
void SPUAssembler::hbrr(U32 ro, U32 i16) { } // TODO: 0x12000000
void SPUAssembler::heq(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x7B000000, rt, ra, rb); }
void SPUAssembler::heqi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x7F000000, rt, ra, i10); }
void SPUAssembler::hgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x4B000000, rt, ra, rb); }
void SPUAssembler::hgti(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x4F000000, rt, ra, i10); }
void SPUAssembler::hlgt(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x5B000000, rt, ra, rb); }
void SPUAssembler::hlgti(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x5F000000, rt, ra, i10); }
void SPUAssembler::il(RegGPR rt, U32 i16) { emitFormRI16(0x40800000, rt, i16); }
void SPUAssembler::ila(RegGPR rt, U32 i18) { emitFormRI18(0x42000000, rt, i18); }
void SPUAssembler::ilh(RegGPR rt, U32 i16) { emitFormRI16(0x41800000, rt, i16); }
void SPUAssembler::ilhu(RegGPR rt, U32 i16) { emitFormRI16(0x41000000, rt, i16); }
void SPUAssembler::iohl(RegGPR rt, U32 i16) { emitFormRI16(0x60800000, rt, i16); }
void SPUAssembler::iret(RegGPR ra) { emitFormRR(0x35400000, 0, ra, 0); } // TODO
void SPUAssembler::lnop() { emitFormRR(0x00200000, 0, 0, 0); }
void SPUAssembler::lqa(RegGPR rt, U32 i16) { emitFormRI16(0x30800000, rt, i16); }
void SPUAssembler::lqd(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x34000000, rt, ra, i10); }
void SPUAssembler::lqr(RegGPR rt, U32 i16) { emitFormRI16(0x33800000, rt, i16); }
void SPUAssembler::lqx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x38800000, rt, ra, rb); }
void SPUAssembler::mfspr(RegGPR rt, RegSPR sa) { emitFormRR(0x01800000, rt, sa, 0); }
void SPUAssembler::mpy(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78800000, rt, ra, rb); }
void SPUAssembler::mpya(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0xC0000000, rt, ra, rb, rc); }
void SPUAssembler::mpyh(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78A00000, rt, ra, rb); }
void SPUAssembler::mpyhh(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78C00000, rt, ra, rb); }
void SPUAssembler::mpyhha(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x68C00000, rt, ra, rb); }
void SPUAssembler::mpyhhau(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x69C00000, rt, ra, rb); }
void SPUAssembler::mpyhhu(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x79C00000, rt, ra, rb); }
void SPUAssembler::mpyi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x74000000, rt, ra, i10); }
void SPUAssembler::mpys(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x78E00000, rt, ra, rb); }
void SPUAssembler::mpyu(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x79800000, rt, ra, rb); }
void SPUAssembler::mpyui(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x75000000, rt, ra, i10); }
void SPUAssembler::mtspr(RegSPR sa, RegGPR rt) { emitFormRR(0x21800000, rt, sa, 0); }
void SPUAssembler::nand(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x19200000, rt, ra, rb); }
void SPUAssembler::nop(RegGPR rt) { emitFormRR(0x40200000, rt, 0, 0); }
void SPUAssembler::nor(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x09200000, rt, ra, rb); }
void SPUAssembler::or_(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x08200000, rt, ra, rb); }
void SPUAssembler::orbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x06000000, rt, ra, i10); }
void SPUAssembler::orc(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x59200000, rt, ra, rb); }
void SPUAssembler::orhi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x05000000, rt, ra, i10); }
void SPUAssembler::ori(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x04000000, rt, ra, i10); }
void SPUAssembler::orx(RegGPR rt, RegGPR ra) { emitFormRR(0x3E000000, rt, ra, 0); }
void SPUAssembler::rchcnt(RegGPR rt, RegGPR ra) { emitFormRR(0x01E00000, rt, ra, 0); }
void SPUAssembler::rdch(RegGPR rt, U32 ca) { emitFormRR(0x01A00000, rt, ca, 0); }
void SPUAssembler::rot(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0B000000, rt, ra, rb); }
void SPUAssembler::roth(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0B800000, rt, ra, rb); }
void SPUAssembler::rothi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0F800000, rt, ra, i7); }
void SPUAssembler::rothm(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0BA00000, rt, ra, rb); }
void SPUAssembler::rothmi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0FA00000, rt, ra, i7); }
void SPUAssembler::roti(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0F000000, rt, ra, i7); }
void SPUAssembler::rotm(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0B200000, rt, ra, rb); }
void SPUAssembler::rotma(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0B400000, rt, ra, rb); }
void SPUAssembler::rotmah(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0BC00000, rt, ra, rb); }
void SPUAssembler::rotmahi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0FC00000, rt, ra, i7); }
void SPUAssembler::rotmai(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0F400000, rt, ra, i7); }
void SPUAssembler::rotmi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0F200000, rt, ra, i7); }
void SPUAssembler::rotqbi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3B000000, rt, ra, rb); }
void SPUAssembler::rotqbii(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3F000000, rt, ra, i7); }
void SPUAssembler::rotqby(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3B800000, rt, ra, rb); }
void SPUAssembler::rotqbybi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x39800000, rt, ra, rb); }
void SPUAssembler::rotqbyi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3F800000, rt, ra, i7); }
void SPUAssembler::rotqmbi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3B200000, rt, ra, rb); }
void SPUAssembler::rotqmbii(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3F200000, rt, ra, i7); }
void SPUAssembler::rotqmby(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3BA00000, rt, ra, rb); }
void SPUAssembler::rotqmbybi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x39A00000, rt, ra, rb); }
void SPUAssembler::rotqmbyi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3FA00000, rt, ra, i7); }
void SPUAssembler::selb(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0x80000000, rt, ra, rb, rc); }
void SPUAssembler::sf(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x08000000, rt, ra, rb); }
void SPUAssembler::sfh(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x09000000, rt, ra, rb); }
void SPUAssembler::sfhi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x0D000000, rt, ra, i10); }
void SPUAssembler::sfi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x0C000000, rt, ra, i10); }
void SPUAssembler::sfx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x68200000, rt, ra, rb); }
void SPUAssembler::shl(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0B600000, rt, ra, rb); }
void SPUAssembler::shlh(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x0BE00000, rt, ra, rb); }
void SPUAssembler::shlhi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0FE00000, rt, ra, i7); }
void SPUAssembler::shli(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x0F600000, rt, ra, i7); }
void SPUAssembler::shlqbi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3B600000, rt, ra, rb); }
void SPUAssembler::shlqbii(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3F600000, rt, ra, i7); }
void SPUAssembler::shlqby(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x3BE00000, rt, ra, rb); }
void SPUAssembler::shlqbybi(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x39E00000, rt, ra, rb); }
void SPUAssembler::shlqbyi(RegGPR rt, RegGPR ra, U32 i7) { emitFormRI7(0x3FE00000, rt, ra, i7); }
void SPUAssembler::shufb(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc) { emitFormRRR(0xB0000000, rt, ra, rb, rc); }
void SPUAssembler::stop() { } // TODO: 0x00000000
void SPUAssembler::stopd() { } // TODO: 0x28000000
void SPUAssembler::stqa(RegGPR rt, U32 i16) { emitFormRI16(0x20800000, rt, i16); }
void SPUAssembler::stqd(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x24000000, rt, ra, i10); }
void SPUAssembler::stqr(RegGPR rt, U32 i16) { emitFormRI16(0x23800000, rt, i16); }
void SPUAssembler::stqx(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x28800000, rt, ra, rb); }
void SPUAssembler::sumb(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x4A600000, rt, ra, rb); }
void SPUAssembler::sync() { } // TODO: 0x00400000
void SPUAssembler::wrch(U32 ca, RegGPR rt) { emitFormRR(0x21A00000, rt, ca, 0); }
void SPUAssembler::xor_(RegGPR rt, RegGPR ra, RegGPR rb) { emitFormRR(0x48200000, rt, ra, rb); }
void SPUAssembler::xorbi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x46000000, rt, ra, i10); }
void SPUAssembler::xorhi(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x45000000, rt, ra, i10); }
void SPUAssembler::xori(RegGPR rt, RegGPR ra, U32 i10) { emitFormRI10(0x44000000, rt, ra, i10); }
void SPUAssembler::xsbh(RegGPR rt, RegGPR ra) { emitFormRR(0x56C00000, rt, ra, 0); }
void SPUAssembler::xshw(RegGPR rt, RegGPR ra) { emitFormRR(0x55C00000, rt, ra, 0); }
void SPUAssembler::xswd(RegGPR rt, RegGPR ra) { emitFormRR(0x54C00000, rt, ra, 0); }

}  // namespace spu
}  // namespace backend
}  // namespace cpu
