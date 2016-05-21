/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/assembler.h"

namespace cpu {
namespace backend {
namespace spu {

using Register = U32;

// General-Purpose Registers
enum RegGPR : Register {
    r0,   r1,   r2,   r3,   r4,   r5,   r6,   r7,
    r8,   r9,   r10,  r11,  r12,  r13,  r14,  r15,
    r16,  r17,  r18,  r19,  r20,  r21,  r22,  r23,
    r24,  r25,  r26,  r27,  r28,  r29,  r30,  r31,
    r32,  r33,  r34,  r35,  r36,  r37,  r38,  r39,
    r40,  r41,  r42,  r43,  r44,  r45,  r46,  r47,
    r48,  r49,  r50,  r51,  r52,  r53,  r54,  r55,
    r56,  r57,  r58,  r59,  r60,  r61,  r62,  r63,
    r64,  r65,  r66,  r67,  r68,  r69,  r70,  r71,
    r72,  r73,  r74,  r75,  r76,  r77,  r78,  r79,
    r80,  r81,  r82,  r83,  r84,  r85,  r86,  r87,
    r88,  r89,  r90,  r91,  r92,  r93,  r94,  r95,
    r96,  r97,  r98,  r99,  r100, r101, r102, r103,
    r104, r105, r106, r107, r108, r109, r110, r111,
    r112, r113, r114, r115, r116, r117, r118, r119,
    r120, r121, r122, r123, r124, r125, r126, r127,
};

// Special-Purpose Registers
enum RegSPR : Register {
    s0,   s1,   s2,   s3,   s4,   s5,   s6,   s7,
    s8,   s9,   s10,  s11,  s12,  s13,  s14,  s15,
    s16,  s17,  s18,  s19,  s20,  s21,  s22,  s23,
    s24,  s25,  s26,  s27,  s28,  s29,  s30,  s31,
    s32,  s33,  s34,  s35,  s36,  s37,  s38,  s39,
    s40,  s41,  s42,  s43,  s44,  s45,  s46,  s47,
    s48,  s49,  s50,  s51,  s52,  s53,  s54,  s55,
    s56,  s57,  s58,  s59,  s60,  s61,  s62,  s63,
    s64,  s65,  s66,  s67,  s68,  s69,  s70,  s71,
    s72,  s73,  s74,  s75,  s76,  s77,  s78,  s79,
    s80,  s81,  s82,  s83,  s84,  s85,  s86,  s87,
    s88,  s89,  s90,  s91,  s92,  s93,  s94,  s95,
    s96,  s97,  s98,  s99,  s100, s101, s102, s103,
    s104, s105, s106, s107, s108, s109, s110, s111,
    s112, s113, s114, s115, s116, s117, s118, s119,
    s120, s121, s122, s123, s124, s125, s126, s127,
};

class SPUAssembler : public Assembler {
private:
    // Operand
    using Operand = U32;

    // Emit instruction form
    void emitFormRR(U32 instruction, Operand rt, Operand ra, Operand rb);
    void emitFormRRR(U32 instruction, Operand rt, Operand ra, Operand rb, Operand rc);
    void emitFormRI7(U32 instruction, Operand rt, Operand ra, U32 i7);
    void emitFormRI8(U32 instruction, Operand rt, Operand ra, U32 i10);
    void emitFormRI10(U32 instruction, Operand rt, Operand ra, U32 i10);
    void emitFormRI16(U32 instruction, Operand rt, U32 i16);
    void emitFormRI18(U32 instruction, Operand rt, U32 i18);

public:
    SPUAssembler(Size codeSize = 4096, void* codeAddr = nullptr)
        : Assembler(codeSize, codeAddr) {}

    // SPU instructions
    void a(RegGPR rt, RegGPR ra, RegGPR rb);
    void absdb(RegGPR rt, RegGPR ra, RegGPR rb);
    void addx(RegGPR rt, RegGPR ra, RegGPR rb);
    void ah(RegGPR rt, RegGPR ra, RegGPR rb);
    void ahi(RegGPR rt, RegGPR ra, U32 i10);
    void ai(RegGPR rt, RegGPR ra, U32 i10);
    void and_(RegGPR rt, RegGPR ra, RegGPR rb);
    void andbi(RegGPR rt, RegGPR ra, U32 i10);
    void andc(RegGPR rt, RegGPR ra, RegGPR rb);
    void andhi(RegGPR rt, RegGPR ra, U32 i10);
    void andi(RegGPR rt, RegGPR ra, U32 i10);
    void avgb(RegGPR rt, RegGPR ra, RegGPR rb);
    void bg(RegGPR rt, RegGPR ra, RegGPR rb);
    void bgx(RegGPR rt, RegGPR ra, RegGPR rb);
    void bi(RegGPR ra);
    void bihnz(RegGPR rt, RegGPR ra);
    void bihz(RegGPR rt, RegGPR ra);
    void binz(RegGPR rt, RegGPR ra);
    void bisl(RegGPR rt, RegGPR ra);
    void bisled(RegGPR rt, RegGPR ra);
    void biz(RegGPR rt, RegGPR ra);
    void br(U32 i16);
    void bra(U32 i16);
    void brasl(RegGPR rt, U32 i16);
    void brhnz(RegGPR rt, U32 i16);
    void brhz(RegGPR rt, U32 i16);
    void brnz(RegGPR rt, U32 i16);
    void brsl(RegGPR rt, U32 i16);
    void brz(RegGPR rt, U32 i16);
    void cbd(RegGPR rt, RegGPR ra, U32 i7);
    void cbx(RegGPR rt, RegGPR ra, RegGPR rb);
    void cdd(RegGPR rt, RegGPR ra, U32 i7);
    void cdx(RegGPR rt, RegGPR ra, RegGPR rb);
    void ceq(RegGPR rt, RegGPR ra, RegGPR rb);
    void ceqb(RegGPR rt, RegGPR ra, RegGPR rb);
    void ceqbi(RegGPR rt, RegGPR ra, U32 i10);
    void ceqh(RegGPR rt, RegGPR ra, RegGPR rb);
    void ceqhi(RegGPR rt, RegGPR ra, U32 i10);
    void ceqi(RegGPR rt, RegGPR ra, U32 i10);
    void cflts(RegGPR rt, RegGPR ra, U32 i8);
    void cfltu(RegGPR rt, RegGPR ra, U32 i8);
    void cg(RegGPR rt, RegGPR ra, RegGPR rb);
    void cgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void cgtb(RegGPR rt, RegGPR ra, RegGPR rb);
    void cgtbi(RegGPR rt, RegGPR ra, U32 i10);
    void cgth(RegGPR rt, RegGPR ra, RegGPR rb);
    void cgthi(RegGPR rt, RegGPR ra, U32 i10);
    void cgti(RegGPR rt, RegGPR ra, U32 i10);
    void cgx(RegGPR rt, RegGPR ra, RegGPR rb);
    void chd(RegGPR rt, RegGPR ra, U32 i7);
    void chx(RegGPR rt, RegGPR ra, RegGPR rb);
    void clgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void clgtb(RegGPR rt, RegGPR ra, RegGPR rb);
    void clgtbi(RegGPR rt, RegGPR ra, U32 i10);
    void clgth(RegGPR rt, RegGPR ra, RegGPR rb);
    void clgthi(RegGPR rt, RegGPR ra, U32 i10);
    void clgti(RegGPR rt, RegGPR ra, U32 i10);
    void clz(RegGPR rt, RegGPR ra);
    void cntb(RegGPR rt, RegGPR ra);
    void csflt(RegGPR rt, RegGPR ra, U32 i8);
    void cuflt(RegGPR rt, RegGPR ra, U32 i8);
    void cwd(RegGPR rt, RegGPR ra, U32 i7);
    void cwx(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfa(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfceq(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfcgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfcmeq(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfcmgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfm(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfma(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfms(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfnma(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfnms(RegGPR rt, RegGPR ra, RegGPR rb);
    void dfs(RegGPR rt, RegGPR ra, RegGPR rb);
    void dftsv(RegGPR rt, RegGPR ra, U32 i7);
    void dsync();
    void eqv(RegGPR rt, RegGPR ra, RegGPR rb);
    void fa(RegGPR rt, RegGPR ra, RegGPR rb);
    void fceq(RegGPR rt, RegGPR ra, RegGPR rb);
    void fcgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void fcmeq(RegGPR rt, RegGPR ra, RegGPR rb);
    void fcmgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void fesd(RegGPR rt, RegGPR ra);
    void fi(RegGPR rt, RegGPR ra, RegGPR rb);
    void fm(RegGPR rt, RegGPR ra, RegGPR rb);
    void fma(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void fms(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void fnms(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void frds(RegGPR rt, RegGPR ra);
    void frest(RegGPR rt, RegGPR ra);
    void frsqest(RegGPR rt, RegGPR ra);
    void fs(RegGPR rt, RegGPR ra, RegGPR rb);
    void fscrrd(RegGPR rt);
    void fscrwr(RegGPR rt, RegGPR ra);
    void fsm(RegGPR rt, RegGPR ra);
    void fsmb(RegGPR rt, RegGPR ra);
    void fsmbi(RegGPR rt, U32 i16);
    void fsmh(RegGPR rt, RegGPR ra);
    void gb(RegGPR rt, RegGPR ra);
    void gbb(RegGPR rt, RegGPR ra);
    void gbh(RegGPR rt, RegGPR ra);
    void hbr(U32 ro, U32 i7);
    void hbra(U32 ro, U32 i16);
    void hbrr(U32 ro, U32 i16);
    void heq(RegGPR rt, RegGPR ra, RegGPR rb);
    void heqi(RegGPR rt, RegGPR ra, U32 i10);
    void hgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void hgti(RegGPR rt, RegGPR ra, U32 i10);
    void hlgt(RegGPR rt, RegGPR ra, RegGPR rb);
    void hlgti(RegGPR rt, RegGPR ra, U32 i10);
    void il(RegGPR rt, U32 i16);
    void ila(RegGPR rt, U32 i18);
    void ilh(RegGPR rt, U32 i16);
    void ilhu(RegGPR rt, U32 i16);
    void iohl(RegGPR rt, U32 i16);
    void iret(RegGPR ra);
    void lnop();
    void lqa(RegGPR rt, U32 i16);
    void lqd(RegGPR rt, RegGPR ra, U32 i10 = 0);
    void lqr(RegGPR rt, U32 i16);
    void lqx(RegGPR rt, RegGPR ra, RegGPR rb);
    void mfspr(RegGPR rt, RegSPR sa);
    void mpy(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpya(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void mpyh(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyhh(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyhha(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyhhau(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyhhu(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyi(RegGPR rt, RegGPR ra, U32 i10);
    void mpys(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyu(RegGPR rt, RegGPR ra, RegGPR rb);
    void mpyui(RegGPR rt, RegGPR ra, U32 i10);
    void mtspr(RegSPR sa, RegGPR rt);
    void nand(RegGPR rt, RegGPR ra, RegGPR rb);
    void nop(RegGPR rt = r0);
    void nor(RegGPR rt, RegGPR ra, RegGPR rb);
    void or_(RegGPR rt, RegGPR ra, RegGPR rb);
    void orbi(RegGPR rt, RegGPR ra, U32 i10);
    void orc(RegGPR rt, RegGPR ra, RegGPR rb);
    void orhi(RegGPR rt, RegGPR ra, U32 i10);
    void ori(RegGPR rt, RegGPR ra, U32 i10);
    void orx(RegGPR rt, RegGPR ra);
    void rchcnt(RegGPR rt, RegGPR ra);
    void rdch(RegGPR rt, RegGPR ra);
    void rot(RegGPR rt, RegGPR ra, RegGPR rb);
    void roth(RegGPR rt, RegGPR ra, RegGPR rb);
    void rothi(RegGPR rt, RegGPR ra, U32 i7);
    void rothm(RegGPR rt, RegGPR ra, RegGPR rb);
    void rothmi(RegGPR rt, RegGPR ra, U32 i7);
    void roti(RegGPR rt, RegGPR ra, U32 i7);
    void rotm(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotma(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotmah(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotmahi(RegGPR rt, RegGPR ra, U32 i7);
    void rotmai(RegGPR rt, RegGPR ra, U32 i7);
    void rotmi(RegGPR rt, RegGPR ra, U32 i7);
    void rotqbi(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqbii(RegGPR rt, RegGPR ra, U32 i7);
    void rotqby(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqbybi(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqbyi(RegGPR rt, RegGPR ra, U32 i7);
    void rotqmbi(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqmbii(RegGPR rt, RegGPR ra, U32 i7);
    void rotqmby(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqmbybi(RegGPR rt, RegGPR ra, RegGPR rb);
    void rotqmbyi(RegGPR rt, RegGPR ra, U32 i7);
    void selb(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void sf(RegGPR rt, RegGPR ra, RegGPR rb);
    void sfh(RegGPR rt, RegGPR ra, RegGPR rb);
    void sfhi(RegGPR rt, RegGPR ra, U32 i10);
    void sfi(RegGPR rt, RegGPR ra, U32 i10);
    void sfx(RegGPR rt, RegGPR ra, RegGPR rb);
    void shl(RegGPR rt, RegGPR ra, RegGPR rb);
    void shlh(RegGPR rt, RegGPR ra, RegGPR rb);
    void shlhi(RegGPR rt, RegGPR ra, U32 i7);
    void shli(RegGPR rt, RegGPR ra, U32 i7);
    void shlqbi(RegGPR rt, RegGPR ra, RegGPR rb);
    void shlqbii(RegGPR rt, RegGPR ra, U32 i7);
    void shlqby(RegGPR rt, RegGPR ra, RegGPR rb);
    void shlqbybi(RegGPR rt, RegGPR ra, RegGPR rb);
    void shlqbyi(RegGPR rt, RegGPR ra, U32 i7);
    void shufb(RegGPR rt, RegGPR ra, RegGPR rb, RegGPR rc);
    void stop(); // TODO: ?
    void stopd(); // TODO: ?
    void stqa(RegGPR rt, U32 i16);
    void stqd(RegGPR rt, RegGPR ra, U32 i10 = 0);
    void stqr(RegGPR rt, U32 i16);
    void stqx(RegGPR rt, RegGPR ra, RegGPR rb);
    void sumb(RegGPR rt, RegGPR ra, RegGPR rb);
    void sync();
    void wrch(RegGPR rt, RegGPR ra);
    void xor_(RegGPR rt, RegGPR ra, RegGPR rb);
    void xorbi(RegGPR rt, RegGPR ra, U32 i10);
    void xorhi(RegGPR rt, RegGPR ra, U32 i10);
    void xori(RegGPR rt, RegGPR ra, U32 i10);
    void xsbh(RegGPR rt, RegGPR ra);
    void xshw(RegGPR rt, RegGPR ra);
    void xswd(RegGPR rt, RegGPR ra);
};

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
