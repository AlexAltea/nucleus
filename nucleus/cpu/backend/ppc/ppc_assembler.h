/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/assembler.h"

namespace cpu {
namespace backend {
namespace ppc {

using Register = U32;

// General-Purpose Registers
enum RegGPR : Register {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
    r8,  r9,  r10, r11, r12, r13, r14, r15,
    r16, r17, r18, r19, r20, r21, r22, r23,
    r24, r25, r26, r27, r28, r29, r30, r31
};

// Floating-Point Registers
enum RegFPR : Register {
    f0,  f1,  f2,  f3,  f4,  f5,  f6,  f7,
    f8,  f9,  f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31
};

// Vector Registers
enum RegVR : Register {
    v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,
    v8,  v9,  v10, v11, v12, v13, v14, v15,
    v16, v17, v18, v19, v20, v21, v22, v23,
    v24, v25, v26, v27, v28, v29, v30, v31
};

enum RegCR : Register {
    cr0, cr1, cr2, cr3, cr4, cr5, cr6, cr7
};

class PPCAssembler : public Assembler {
private:
    // Operand
    using Operand = U32;

    // Emit instruction
    void emit(U32 instruction);

    // Emit instruction form
    void emitFormI(Operand opcd, Operand li, bool aa, bool lk);
    void emitFormB(Operand opcd, Operand bo, Operand bi, Operand bd, bool aa, bool lk);
    void emitFormSC(Operand opcd);
    void emitFormD(U32 instruction, Operand d, Operand a, U16 imm);
    void emitFormDS(U32 instruction, Operand d, Operand a, U16 imm);
    void emitFormX(U32 instruction, Operand d, Operand a, Operand b);
    void emitFormXL(Operand opcd, Operand d, Operand a, Operand b, Operand xo, bool lk);
    void emitFormXFX(Operand opcd, Operand d, Operand spr, Operand xo);
    void emitFormXFL(Operand opcd, Operand fm, Operand b, Operand xo, bool rc);
    void emitFormXS(Operand opcd, Operand s, Operand a, Operand sh, Operand xo, bool rc);
    void emitFormXO(U32 instruction, Operand d, Operand a, Operand b);
    void emitFormA(Operand opcd, Operand d, Operand a, Operand b, Operand c, Operand xo, bool rc);

public:
    PPCAssembler(size_t codeSize = 4096, void* codeAddr = nullptr);

    // PPC instructions
    void add(RegGPR rd, RegGPR ra, RegGPR rb);
    void add_(RegGPR rd, RegGPR ra, RegGPR rb);
    void addc(RegGPR rd, RegGPR ra, RegGPR rb);
    void addc_(RegGPR rd, RegGPR ra, RegGPR rb);
    void addco(RegGPR rd, RegGPR ra, RegGPR rb);
    void addco_(RegGPR rd, RegGPR ra, RegGPR rb);
    void adde(RegGPR rd, RegGPR ra, RegGPR rb);
    void adde_(RegGPR rd, RegGPR ra, RegGPR rb);
    void addeo(RegGPR rd, RegGPR ra, RegGPR rb);
    void addeo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void addi(RegGPR rd, RegGPR ra, S16 simm);
    void addic(RegGPR rd, RegGPR ra, S16 simm);
    void addic_(RegGPR rd, RegGPR ra, S16 simm);
    void addis(RegGPR rd, RegGPR ra, S16 simm);
    void addme(RegGPR rd, RegGPR ra);
    void addme_(RegGPR rd, RegGPR ra);
    void addmeo(RegGPR rd, RegGPR ra);
    void addmeo_(RegGPR rd, RegGPR ra);
    void addo(RegGPR rd, RegGPR ra, RegGPR rb);
    void addo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void addze(RegGPR rd, RegGPR ra);
    void addze_(RegGPR rd, RegGPR ra);
    void addzeo(RegGPR rd, RegGPR ra);
    void addzeo_(RegGPR rd, RegGPR ra);
    void and_(RegGPR ra, RegGPR rs, RegGPR rb);
    void and__(RegGPR ra, RegGPR rs, RegGPR rb);
    void andc(RegGPR ra, RegGPR rs, RegGPR rb);
    void andc_(RegGPR ra, RegGPR rs, RegGPR rb);
    void andi_(RegGPR ra, RegGPR rs, U16 uimm);
    void andis_(RegGPR ra, RegGPR rs, U16 uimm);
    void bcctrx(); // TODO
    void bclrx(); // TODO
    void bcx(); // TODO
    void bx(); // TODO
    void cmp(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmp(RegGPR ra, RegGPR rb);
    void cmpd(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmpd(RegGPR ra, RegGPR rb);
    void cmpdi(RegCR crfd, RegGPR ra, S16 simm);
    void cmpdi(RegGPR ra, S16 simm);
    void cmpi(RegCR crfd, RegGPR ra, S16 simm);
    void cmpi(RegGPR ra, S16 simm);
    void cmpl(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmpl(RegGPR ra, RegGPR rb);
    void cmpld(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmpld(RegGPR ra, RegGPR rb);
    void cmpldi(RegCR crfd, RegGPR ra, S16 simm);
    void cmpldi(RegGPR ra, S16 simm);
    void cmpli(RegCR crfd, RegGPR ra, S16 simm);
    void cmpli(RegGPR ra, S16 simm);
    void cmplw(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmplw(RegGPR ra, RegGPR rb);
    void cmplwi(RegCR crfd, RegGPR ra, S16 simm);
    void cmplwi(RegGPR ra, S16 simm);
    void cmpw(RegCR crfd, RegGPR ra, RegGPR rb);
    void cmpw(RegGPR ra, RegGPR rb);
    void cmpwi(RegCR crfd, RegGPR ra, S16 simm);
    void cmpwi(RegGPR ra, S16 simm);
    void cntlzd(RegGPR ra, RegGPR rs);
    void cntlzd_(RegGPR ra, RegGPR rs);
    void cntlzw(RegGPR ra, RegGPR rs);
    void cntlzw_(RegGPR ra, RegGPR rs);
    void crand(RegCR crfd, RegCR crfa, RegCR crfb);
    void crandc(RegCR crfd, RegCR crfa, RegCR crfb);
    void creqv(RegCR crfd, RegCR crfa, RegCR crfb);
    void crnand(RegCR crfd, RegCR crfa, RegCR crfb);
    void crnor(RegCR crfd, RegCR crfa, RegCR crfb);
    void cror(RegCR crfd, RegCR crfa, RegCR crfb);
    void crorc(RegCR crfd, RegCR crfa, RegCR crfb);
    void crxor(RegCR crfd, RegCR crfa, RegCR crfb);
    void dcbf(RegGPR ra, RegGPR rb);
    void dcbst(RegGPR ra, RegGPR rb);
    void dcbt(); // TODO
    void dcbtst(RegGPR ra, RegGPR rb);
    void dcbz(RegGPR ra, RegGPR rb);
    void divd(RegGPR rd, RegGPR ra, RegGPR rb);
    void divd_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divdo(RegGPR rd, RegGPR ra, RegGPR rb);
    void divdo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divdu(RegGPR rd, RegGPR ra, RegGPR rb);
    void divdu_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divduo(RegGPR rd, RegGPR ra, RegGPR rb);
    void divduo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divw(RegGPR rd, RegGPR ra, RegGPR rb);
    void divw_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwo(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwu(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwu_(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwuo(RegGPR rd, RegGPR ra, RegGPR rb);
    void divwuo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void eciwx(RegGPR rd, RegGPR ra, RegGPR rb);
    void ecowx(RegGPR rs, RegGPR ra, RegGPR rb);
    void eieio();
    void eqv(RegGPR ra, RegGPR rs, RegGPR rb);
    void eqv_(RegGPR ra, RegGPR rs, RegGPR rb);
    void extsb(RegGPR ra, RegGPR rs);
    void extsb_(RegGPR ra, RegGPR rs);
    void extsh(RegGPR ra, RegGPR rs);
    void extsh_(RegGPR ra, RegGPR rs);
    void extsw(RegGPR ra, RegGPR rs);
    void extsw_(RegGPR ra, RegGPR rs);
    void fabs(RegFPR frd, RegFPR frb);
    void fabs_(RegFPR frd, RegFPR frb);
    void fadd(RegFPR frd, RegFPR fra, RegFPR frb);
    void fadd_(RegFPR frd, RegFPR fra, RegFPR frb);
    void fadds(RegFPR frd, RegFPR fra, RegFPR frb);
    void fadds_(RegFPR frd, RegFPR fra, RegFPR frb);
    void fcfid(RegFPR frd, RegFPR frb);
    void fcfid_(RegFPR frd, RegFPR frb);
    void fcmpo(RegCR crfd, RegFPR fra, RegFPR frb);
    void fcmpu(RegCR crfd, RegFPR fra, RegFPR frb);
    void fctid(RegFPR frd, RegFPR frb);
    void fctid_(RegFPR frd, RegFPR frb);
    void fctidz(RegFPR frd, RegFPR frb);
    void fctidz_(RegFPR frd, RegFPR frb);
    void fctiw(RegFPR frd, RegFPR frb);
    void fctiw_(RegFPR frd, RegFPR frb);
    void fctiwz(RegFPR frd, RegFPR frb);
    void fctiwz_(RegFPR frd, RegFPR frb);
    void fdiv(RegFPR frd, RegFPR fra, RegFPR frb);
    void fdiv_(RegFPR frd, RegFPR fra, RegFPR frb);
    void fdivs(RegFPR frd, RegFPR fra, RegFPR frb);
    void fdivs_(RegFPR frd, RegFPR fra, RegFPR frb);
    void fmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmr(RegFPR frd, RegFPR frb);
    void fmr_(RegFPR frd, RegFPR frb);
    void fmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fmul(RegFPR frd, RegFPR fra, RegFPR frc);
    void fmul_(RegFPR frd, RegFPR fra, RegFPR frc);
    void fmuls(RegFPR frd, RegFPR fra, RegFPR frc);
    void fmuls_(RegFPR frd, RegFPR fra, RegFPR frc);
    void fnabs(RegFPR frd, RegFPR frb);
    void fnabs_(RegFPR frd, RegFPR frb);
    void fneg(RegFPR frd, RegFPR frb);
    void fneg_(RegFPR frd, RegFPR frb);
    void fnmadd(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmadd_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmadds(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmadds_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmsub(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmsub_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmsubs(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fnmsubs_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fres(RegFPR frd, RegFPR frb);
    void fres_(RegFPR frd, RegFPR frb);
    void frsp(RegFPR frd, RegFPR frb);
    void frsp_(RegFPR frd, RegFPR frb);
    void frsqrte(RegFPR frd, RegFPR frb);
    void frsqrte_(RegFPR frd, RegFPR frb);
    void fsel(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fsel_(RegFPR frd, RegFPR fra, RegFPR frc, RegFPR frb);
    void fsqrt(RegFPR frd, RegFPR frb);
    void fsqrt_(RegFPR frd, RegFPR frb);
    void fsqrts(RegFPR frd, RegFPR frb);
    void fsqrts_(RegFPR frd, RegFPR frb);
    void fsub(RegFPR frd, RegFPR fra, RegFPR frb);
    void fsub_(RegFPR frd, RegFPR fra, RegFPR frb);
    void fsubs(RegFPR frd, RegFPR fra, RegFPR frb);
    void fsubs_(RegFPR frd, RegFPR fra, RegFPR frb);
    void icbi(RegGPR ra, RegGPR rb);
    void isync();
    void lbz(RegGPR rd, RegGPR ra, U16 d = 0);
    void lbzu(RegGPR rd, RegGPR ra, U16 d = 0);
    void lbzux(RegGPR rd, RegGPR ra, RegGPR rb);
    void lbzx(RegGPR rd, RegGPR ra, RegGPR rb);
    void ld(RegGPR rd, RegGPR ra, U16 ds = 0);
    void ldarx(RegGPR rd, RegGPR ra, RegGPR rb);
    void ldbrx(); // TODO
    void ldu(RegGPR rd, RegGPR ra, U16 ds = 0);
    void ldux(RegGPR rd, RegGPR ra, RegGPR rb);
    void ldx(RegGPR rd, RegGPR ra, RegGPR rb);
    void lfd(RegFPR frd, RegGPR ra, U16 d = 0);
    void lfdu(RegFPR frd, RegGPR ra, U16 d = 0);
    void lfdux(RegFPR frd, RegGPR ra, RegGPR rb);
    void lfdx(RegFPR frd, RegGPR ra, RegGPR rb);
    void lfs(RegFPR frd, RegGPR ra, U16 d = 0);
    void lfsu(RegFPR frd, RegGPR ra, U16 d = 0);
    void lfsux(RegFPR frd, RegGPR ra, RegGPR rb);
    void lfsx(RegFPR frd, RegGPR ra, RegGPR rb);
    void lha(RegGPR rd, RegGPR ra, U16 d = 0);
    void lhau(RegGPR rd, RegGPR ra, U16 d = 0);
    void lhaux(RegGPR rd, RegGPR ra, RegGPR rb);
    void lhax(RegGPR rd, RegGPR ra, RegGPR rb);
    void lhbrx(RegGPR rd, RegGPR ra, RegGPR rb);
    void lhz(RegGPR rd, RegGPR ra, U16 d = 0);
    void lhzu(RegGPR rd, RegGPR ra, U16 d = 0);
    void lhzux(RegGPR rd, RegGPR ra, RegGPR rb);
    void lhzx(RegGPR rd, RegGPR ra, RegGPR rb);
    void li(RegGPR rd, S16 simm);
    void lis(RegGPR rd, S16 simm);
    void lmw(RegGPR rd, RegGPR ra, U16 d = 0);
    void lswi(); // TODO
    void lswx(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwa(RegGPR rd, RegGPR ra, U16 ds = 0);
    void lwarx(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwaux(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwax(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwbrx(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwz(RegGPR rd, RegGPR ra, U16 d = 0);
    void lwzu(RegGPR rd, RegGPR ra, U16 d = 0);
    void lwzux(RegGPR rd, RegGPR ra, RegGPR rb);
    void lwzx(RegGPR rd, RegGPR ra, RegGPR rb);
    void mcrf(RegCR crfd, RegCR crfs);
    void mcrfs(RegCR crfd, RegCR crfs);
    void mffs(); // TODO
    void mffs_(); // TODO
    void mfocrf(); // TODO
    void mfspr(); // TODO
    void mftb(); // TODO
    void mr(RegGPR ra, RegGPR rs);
    void mtfsb0x(); // TODO
    void mtfsb1x(); // TODO
    void mtfsfix(); // TODO
    void mtfsfx(); // TODO
    void mtocrf(); // TODO
    void mtspr(); // TODO
    void mulhd(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhd_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhdu(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhdu_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhw(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhw_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhwu(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulhwu_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulld(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulld_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulldo(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulldo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mulli(RegGPR rd, RegGPR ra, S16 simm);
    void mullw(RegGPR rd, RegGPR ra, RegGPR rb);
    void mullw_(RegGPR rd, RegGPR ra, RegGPR rb);
    void mullwo(RegGPR rd, RegGPR ra, RegGPR rb);
    void mullwo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void nand(RegGPR ra, RegGPR rs, RegGPR rb);
    void nand_(RegGPR ra, RegGPR rs, RegGPR rb);
    void neg(RegGPR rd, RegGPR ra);
    void neg_(RegGPR rd, RegGPR ra);
    void nego(RegGPR rd, RegGPR ra);
    void nego_(RegGPR rd, RegGPR ra);
    void nop();
    void nor(RegGPR ra, RegGPR rs, RegGPR rb);
    void nor_(RegGPR ra, RegGPR rs, RegGPR rb);
    void not_(RegGPR rd, RegGPR rs );
    void or_(RegGPR ra, RegGPR rs, RegGPR rb);
    void or__(RegGPR ra, RegGPR rs, RegGPR rb);
    void orc(RegGPR ra, RegGPR rs, RegGPR rb);
    void orc_(RegGPR ra, RegGPR rs, RegGPR rb);
    void ori(RegGPR ra, RegGPR rs, U16 uimm);
    void oris(RegGPR ra, RegGPR rs, U16 uimm);
    void rldcl(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb);
    void rldcl_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb);
    void rldcr(RegGPR ra, RegGPR rs, RegGPR rb, U8 me);
    void rldcr_(RegGPR ra, RegGPR rs, RegGPR rb, U8 me);
    void rldic(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rldic_(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rldicl(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rldicl_(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rldicr(RegGPR ra, RegGPR rs, U8 sh, U8 me);
    void rldicr_(RegGPR ra, RegGPR rs, U8 sh, U8 me);
    void rldimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rldimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb);
    void rlwimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me);
    void rlwimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me);
    void rlwinm(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me);
    void rlwinm_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me);
    void rlwnm(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me);
    void rlwnm_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me);
    void sc();
    void sld(RegGPR ra, RegGPR rs, RegGPR rb);
    void sld_(RegGPR ra, RegGPR rs, RegGPR rb);
    void slw(RegGPR ra, RegGPR rs, RegGPR rb);
    void slw_(RegGPR ra, RegGPR rs, RegGPR rb);
    void srad(RegGPR ra, RegGPR rs, RegGPR rb);
    void srad_(RegGPR ra, RegGPR rs, RegGPR rb);
    void sradi(RegGPR ra, RegGPR rs, U8 sh);
    void sradi_(RegGPR ra, RegGPR rs, U8 sh);
    void sraw(RegGPR ra, RegGPR rs, RegGPR rb);
    void sraw_(RegGPR ra, RegGPR rs, RegGPR rb);
    void srawi(RegGPR ra, RegGPR rs, U8 sh);
    void srawi_(RegGPR ra, RegGPR rs, U8 sh);
    void srd(RegGPR ra, RegGPR rs, RegGPR rb);
    void srd_(RegGPR ra, RegGPR rs, RegGPR rb);
    void srw(RegGPR ra, RegGPR rs, RegGPR rb);
    void srw_(RegGPR ra, RegGPR rs, RegGPR rb);
    void stb(RegGPR rs, RegGPR ra, U16 d = 0);
    void stbu(RegGPR rs, RegGPR ra, U16 d = 0);
    void stbux(RegGPR rs, RegGPR ra, RegGPR rb);
    void stbx(RegGPR rs, RegGPR ra, RegGPR rb);
    void std(RegGPR rs, RegGPR ra, U16 ds = 0);
    void stdcx_(RegGPR rs, RegGPR ra, RegGPR rb);
    void stdu(RegGPR rs, RegGPR ra, U16 ds = 0);
    void stdux(RegGPR rs, RegGPR ra, RegGPR rb);
    void stdx(RegGPR rs, RegGPR ra, RegGPR rb);
    void stfd(RegFPR rs, RegGPR ra, U16 d = 0);
    void stfdu(RegFPR frs, RegGPR ra, U16 d = 0);
    void stfdux(RegFPR frs, RegGPR ra, RegGPR rb);
    void stfdx(RegFPR frs, RegGPR ra, RegGPR rb);
    void stfiwx(RegFPR frs, RegGPR ra, RegGPR rb);
    void stfs(RegFPR frs, RegGPR ra, U16 d = 0);
    void stfsu(RegFPR frs, RegGPR ra, U16 d = 0);
    void stfsux(RegFPR frs, RegGPR ra, RegGPR rb);
    void stfsx(RegFPR frs, RegGPR ra, RegGPR rb);
    void sth(RegGPR rs, RegGPR ra, U16 d = 0);
    void sthbrx(RegGPR rs, RegGPR ra, RegGPR rb);
    void sthu(RegGPR rs, RegGPR ra, U16 d = 0);
    void sthux(RegGPR rs, RegGPR ra, RegGPR rb);
    void sthx(RegGPR rs, RegGPR ra, RegGPR rb);
    void stmw(RegGPR rs, RegGPR ra, U16 d = 0);
    void stswi(); // TODO
    void stswx(RegGPR rs, RegGPR ra, RegGPR rb);
    void stw(RegGPR rs, RegGPR ra, U16 d = 0);
    void stwbrx(RegGPR rs, RegGPR ra, RegGPR rb);
    void stwcx_(RegGPR rs, RegGPR ra, RegGPR rb);
    void stwu(RegGPR rs, RegGPR ra, U16 d = 0);
    void stwux(RegGPR rs, RegGPR ra, RegGPR rb);
    void stwx(RegGPR rs, RegGPR ra, RegGPR rb);
    void subf(RegGPR rd, RegGPR ra, RegGPR rb);
    void subf_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfc(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfc_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfco(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfco_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfe(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfe_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfeo(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfeo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfic(RegGPR rd, RegGPR ra, S16 simm);
    void subfme(RegGPR rd, RegGPR ra);
    void subfme_(RegGPR rd, RegGPR ra);
    void subfmeo(RegGPR rd, RegGPR ra);
    void subfmeo_(RegGPR rd, RegGPR ra);
    void subfo(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfo_(RegGPR rd, RegGPR ra, RegGPR rb);
    void subfze(RegGPR rd, RegGPR ra);
    void subfze_(RegGPR rd, RegGPR ra);
    void subfzeo(RegGPR rd, RegGPR ra);
    void subfzeo_(RegGPR rd, RegGPR ra);
    void sync();
    void td(); // TODO
    void tdi(); // TODO
    void tw(); // TODO
    void twi(); // TODO
    void xor_(RegGPR ra, RegGPR rs, RegGPR rb);
    void xor__(RegGPR ra, RegGPR rs, RegGPR rb);
    void xori(RegGPR ra, RegGPR rs, U16 uimm);
    void xoris(RegGPR ra, RegGPR rs, U16 uimm);

    // PPC64 Vector/SIMD Instructions (aka AltiVec)
    void dss();
    void dst();
    void dstst();
    void lvebx();
    void lvehx();
    void lvewx();
    void lvlx();
    void lvlxl();
    void lvrx();
    void lvrxl();
    void lvsl();
    void lvsr();
    void lvx();
    void lvxl();
    void mfvscr();
    void mtvscr();
    void stvebx();
    void stvehx();
    void stvewx();
    void stvlx();
    void stvlxl();
    void stvrx();
    void stvrxl();
    void stvx();
    void stvxl();
    void vaddcuw();
    void vaddfp();
    void vaddsbs();
    void vaddshs();
    void vaddsws();
    void vaddubm();
    void vaddubs();
    void vadduhm();
    void vadduhs();
    void vadduwm();
    void vadduws();
    void vand();
    void vandc();
    void vavgsb();
    void vavgsh();
    void vavgsw();
    void vavgub();
    void vavguh();
    void vavguw();
    void vcfsx();
    void vcfux();
    void vcmpbfp();
    void vcmpbfp_();
    void vcmpeqfp();
    void vcmpeqfp_();
    void vcmpequb();
    void vcmpequb_();
    void vcmpequh();
    void vcmpequh_();
    void vcmpequw();
    void vcmpequw_();
    void vcmpgefp();
    void vcmpgefp_();
    void vcmpgtfp();
    void vcmpgtfp_();
    void vcmpgtsb();
    void vcmpgtsb_();
    void vcmpgtsh();
    void vcmpgtsh_();
    void vcmpgtsw();
    void vcmpgtsw_();
    void vcmpgtub();
    void vcmpgtub_();
    void vcmpgtuh();
    void vcmpgtuh_();
    void vcmpgtuw();
    void vcmpgtuw_();
    void vctsxs();
    void vctuxs();
    void vexptefp();
    void vlogefp();
    void vmaddfp();
    void vmaxfp();
    void vmaxsb();
    void vmaxsh();
    void vmaxsw();
    void vmaxub();
    void vmaxuh();
    void vmaxuw();
    void vmhaddshs();
    void vmhraddshs();
    void vminfp();
    void vminsb();
    void vminsh();
    void vminsw();
    void vminub();
    void vminuh();
    void vminuw();
    void vmladduhm();
    void vmrghb();
    void vmrghh();
    void vmrghw();
    void vmrglb();
    void vmrglh();
    void vmrglw();
    void vmsummbm();
    void vmsumshm();
    void vmsumshs();
    void vmsumubm();
    void vmsumuhm();
    void vmsumuhs();
    void vmulesb();
    void vmulesh();
    void vmuleub();
    void vmuleuh();
    void vmulosb();
    void vmulosh();
    void vmuloub();
    void vmulouh();
    void vnmsubfp();
    void vnor();
    void vor();
    void vperm();
    void vpkpx();
    void vpkshss();
    void vpkshus();
    void vpkswss();
    void vpkswus();
    void vpkuhum();
    void vpkuhus();
    void vpkuwum();
    void vpkuwus();
    void vrefp();
    void vrfim();
    void vrfin();
    void vrfip();
    void vrfiz();
    void vrlb();
    void vrlh();
    void vrlw();
    void vrsqrtefp();
    void vsel();
    void vsl();
    void vslb();
    void vsldoi();
    void vslh();
    void vslo();
    void vslw();
    void vspltb();
    void vsplth();
    void vspltisb();
    void vspltish();
    void vspltisw();
    void vspltw();
    void vsr();
    void vsrab();
    void vsrah();
    void vsraw();
    void vsrb();
    void vsrh();
    void vsro();
    void vsrw();
    void vsubcuw();
    void vsubfp();
    void vsubsbs();
    void vsubshs();
    void vsubsws();
    void vsububm();
    void vsububs();
    void vsubuhm();
    void vsubuhs();
    void vsubuwm();
    void vsubuws();
    void vsum2sws();
    void vsum4sbs();
    void vsum4shs();
    void vsum4ubs();
    void vsumsws();
    void vupkhpx();
    void vupkhsb();
    void vupkhsh();
    void vupklpx();
    void vupklsb();
    void vupklsh();
    void vxor();
};

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
