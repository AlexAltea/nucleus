/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_spu.h"

/**
 * SPU Instructions:
 *  - Constant-Formation Instructions (Chapter 4)
 *  - Integer and Logical Instructions (Chapter 5)
 *  - Shift and Rotate Instructions (Chapter 6)
 */

// Constant-Formation Instructions (Chapter 4)
void SPUTestRunner::fsmbi() {
    // Form Select Mask for Bytes Immediate
    TEST_INSTRUCTION(test_fsmbi, I16, R1, {
        run({ a.fsmbi(r1, I16); });
        expect(state.r[1] == R1);
    });

    // TODO: Is the order correct?
    //test_fsmbi(0x0000, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    //test_fsmbi(0x0001, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x000000FF));
    //test_fsmbi(0x1234, V128::from_u32(0x000000FF, 0x0000FF00, 0x0000FFFF, 0x00FF0000));
    //test_fsmbi(0xFFFF, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void SPUTestRunner::il() {
    // Immediate Load Word
    TEST_INSTRUCTION(test_il, I16, R1, {
        run({ a.il(r1, I16); });
        expect(state.r[1] == R1);
    });

    test_il(0x0000, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_il(0x0001, V128::from_u32(0x00000001, 0x00000001, 0x00000001, 0x00000001));
    test_il(0xFFFF, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void SPUTestRunner::ila() {
    // Immediate Load Address
    TEST_INSTRUCTION(test_ila, I18, R1, {
        run({ a.ila(r1, I18); });
        expect(state.r[1] == R1);
    });

    test_ila(0x00000, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_ila(0x00001, V128::from_u32(0x00000001, 0x00000001, 0x00000001, 0x00000001));
    test_ila(0x3FFFF, V128::from_u32(0x0003FFFF, 0x0003FFFF, 0x0003FFFF, 0x0003FFFF));
}

void SPUTestRunner::ilh() {
    // Immediate Load Halfword
    TEST_INSTRUCTION(test_ilh, I16, R1, {
        run({ a.ilh(r1, I16); });
        expect(state.r[1] == R1);
    });

    test_ilh(0x0000, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_ilh(0x0001, V128::from_u32(0x00010001, 0x00010001, 0x00010001, 0x00010001));
    test_ilh(0xFFFF, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void SPUTestRunner::ilhu() {
    // Immediate Load Halfword Upper
    TEST_INSTRUCTION(test_ilhu, I16, R1, {
        run({ a.ilhu(r1, I16); });
        expect(state.r[1] == R1);
    });

    test_ilhu(0x0000, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_ilhu(0x0001, V128::from_u32(0x00010000, 0x00010000, 0x00010000, 0x00010000));
    test_ilhu(0xFFFF, V128::from_u32(0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000));
}

void SPUTestRunner::iohl() {
    // Immediate Or Halfword Lower
    TEST_INSTRUCTION(test_iohl, I16, oldR1, newR1, {
        state.r[1] = oldR1;
        run({ a.iohl(r1, I16); });
        expect(state.r[1] == newR1);
    });

    test_iohl(0x0000,
        V128::from_u32(0x00000000, 0x00000001, 0x00000000, 0x00000001),
        V128::from_u32(0x00000000, 0x00000001, 0x00000000, 0x00000001));
    test_iohl(0x0001,
        V128::from_u32(0x12345678, 0x00000000, 0x12345678, 0x00000000),
        V128::from_u32(0x12345679, 0x00000001, 0x12345679, 0x00000001));
    test_iohl(0xFFFF,
        V128::from_u32(0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0),
        V128::from_u32(0xF0F0FFFF, 0xF0F0FFFF, 0xF0F0FFFF, 0xF0F0FFFF));
}


// Integer and Logical Instructions (Chapter 5)
void SPUTestRunner::a() {
    // Add Word
    TEST_INSTRUCTION(test_a, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.a(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_a(
        V128::from_u32(0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000001, 0x00000000, 0x00000001),
        V128::from_u32(0x00000000, 0x00000001, 0x00000001, 0x00000000));
    test_a(
        V128::from_u32(0x12345678, 0x12345678, 0x12345678, 0x12345678),
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0x00001000),
        V128::from_u32(0xBCF02355, 0x92345678, 0xBCF02355, 0x12346678));
}

void SPUTestRunner::absdb() {
    // Absolute Differences of Bytes
    TEST_INSTRUCTION(test_absdb, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.absdb(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_absdb(
        V128::from_u32(0x000100FF, 0x8080010F, 0x12345678, 0x8020FF0E),
        V128::from_u32(0x00000100, 0x8020FF0E, 0x12345678, 0x8080010F),
        V128::from_u32(0x000101FF, 0x0060FE01, 0x00000000, 0x0060FE01));
}

void SPUTestRunner::addx() {
    // Add Extended
    TEST_INSTRUCTION(test_addx, RA, RB, oldRT, newRT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = oldRT;
        run({ a.addx(r3, r1, r2); });
        expect(state.r[3] == newRT);
    });

    test_addx(
        V128::from_u32(0x12345678, 0x12345678, 0x12345678, 0x12345678),
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0x00001000),
        V128::from_u32(0x00000000, 0x00000001, 0xFFFFFFFE, 0x11111111),
        V128::from_u32(0xBCF02355, 0x92345679, 0xBCF02355, 0x12346679));
}

void SPUTestRunner::ah() {
    // Add Halfword
    TEST_INSTRUCTION(test_ah, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.ah(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_ah(
        V128::from_u32(0x0000FFFF, 0x00010000, 0x00000001, 0xFFFFFFFF),
        V128::from_u32(0x00000001, 0x00010001, 0xFFFF0000, 0x00000001),
        V128::from_u32(0x00000000, 0x00020001, 0xFFFF0001, 0xFFFF0000));
    test_ah(
        V128::from_u32(0x12345678, 0x12345678, 0x12345678, 0x12345678),
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0xBCEF2355, 0x92345678, 0xBCEF2355, 0x12336678));
}

void SPUTestRunner::ahi() {
    // Add Halfword Immediate
    TEST_INSTRUCTION(test_ahi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.ahi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    test_ahi(0x0000,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000));
    test_ahi(0x0001,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0xAABCCCDE, 0x80010001, 0xAABCCCDE, 0x00001001));
    test_ahi(0x03FF,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0xAABACCDC, 0x7FFFFFFF, 0xAABACCDC, 0xFFFE0FFF));
}

void SPUTestRunner::ai() {
    // Add Word Immediate
    TEST_INSTRUCTION(test_ai, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.ai(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    test_ai(0x0000,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0x00000000, 0xFFFF1000),
        V128::from_u32(0xAABBCCDD, 0x80000000, 0x00000000, 0xFFFF1000));
    test_ai(0x0001,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0x00000000, 0xFFFF1000),
        V128::from_u32(0xAABBCCDE, 0x80000001, 0x00000001, 0xFFFF1001));
    test_ai(0x03FF,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0x00000000, 0xFFFF1000),
        V128::from_u32(0xAABBCCDC, 0x7FFFFFFF, 0xFFFFFFFF, 0xFFFF0FFF));
}

void SPUTestRunner::and_() {
    // And
    TEST_INSTRUCTION(test_and, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.and_(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_and(
        V128::from_u32(0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0FFF),
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0x0000F0F0, 0x11110000, 0x00000000, 0x12480124));
}

void SPUTestRunner::andc() {
    // And with Complement
    TEST_INSTRUCTION(test_andc, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.andc(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_andc(
        V128::from_u32(0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0FFF),
        V128::from_u32(0x0F0F0F0F, 0xEEEEEEEE, 0x0000FFFF, 0xEDB70EDB),
        V128::from_u32(0x0000F0F0, 0x11110000, 0x00000000, 0x12480124));
}

void SPUTestRunner::andbi() {
    // And Byte Immediate
    TEST_INSTRUCTION(test_andbi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.andbi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    test_andbi(0x0000,
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_andbi(0x0001,
        V128::from_u32(0xAABBCCDD, 0x80000000, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0x00010001, 0x00000000, 0x00010001, 0x01010000));
    test_andbi(0x03F0,
        V128::from_u32(0xAABBCCDD, 0x8000FFFF, 0xAABBCCDD, 0xFFFF1000),
        V128::from_u32(0xA0B0C0D0, 0x8000F0F0, 0xA0B0C0D0, 0xF0F01000));
    test_andbi(0x03FF,
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124));
}

void SPUTestRunner::andhi() {
    // And Halfword Immediate
    TEST_INSTRUCTION(test_andhi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.andhi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    test_andhi(0x0000,
        V128::from_u32(0xF0F0F0F0, 0x1111FFFF, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_andhi(0x0001,
        V128::from_u32(0xAABBCCDD, 0xFFFFFFFF, 0x12345678, 0xFFFF1000),
        V128::from_u32(0x00010001, 0x00010001, 0x00000000, 0x00010000));
    test_andhi(0x03FF,
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124));
}

void SPUTestRunner::andi() {
    // And Word Immediate
    TEST_INSTRUCTION(test_andi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.andi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    test_andi(0x0000,
        V128::from_u32(0xF0F0F0F0, 0x1111FFFF, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_andi(0x0001,
        V128::from_u32(0xAABBCCDD, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFF0001),
        V128::from_u32(0x00000001, 0x00000001, 0x00000000, 0x00000001));
    test_andi(0x03FF,
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124),
        V128::from_u32(0xF0F0F0F0, 0x11111111, 0xFFFF0000, 0x1248F124));
}

void SPUTestRunner::avgb() {
    // Average Bytes
    TEST_INSTRUCTION(test_avgb, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.avgb(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    test_avgb(
        V128::from_u32(0x0000FFFF, 0x10203040, 0x01010203, 0xFFFFFFFF),
        V128::from_u32(0x000100FF, 0x08101010, 0x02030101, 0x00010203),
        V128::from_u32(0x000180FF, 0x0C182028, 0x02020202, 0x80808181));
}

void SPUTestRunner::bg() {
    // Borrow Generate
    TEST_INSTRUCTION(test_bg, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.bg(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::bgx() {
    // Borrow Generate Extended
    TEST_INSTRUCTION(test_bgx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.bgx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::cg() {
    // Carry Generate
    TEST_INSTRUCTION(test_cg, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.cg(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::cgx() {
    // Carry Generate Extended
    TEST_INSTRUCTION(test_cgx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.cgx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::clz() {
    // Count Leading Zeros
    TEST_INSTRUCTION(test_clz, RA, RT, {
        state.r[1] = RA;
        run({ a.clz(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::cntb() {
    // Count Ones in Bytes
    TEST_INSTRUCTION(test_cntb, RA, RT, {
        state.r[1] = RA;
        run({ a.cntb(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::eqv() {
    // Equivalent
    TEST_INSTRUCTION(test_eqv, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.eqv(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fsm() {
    // Form Select Mask for Words
    TEST_INSTRUCTION(test_fsm, RA, RT, {
        state.r[1] = RA;
        run({ a.fsm(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::fsmb() {
    // Form Select Mask for Bytes
    TEST_INSTRUCTION(test_fsmb, RA, RT, {
        state.r[1] = RA;
        run({ a.fsmb(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::fsmh() {
    // Form Select Mask for Halfwords
    TEST_INSTRUCTION(test_fsmh, RA, RT, {
        state.r[1] = RA;
        run({ a.fsmh(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::gb() {
    // Gather Bits from Words
    TEST_INSTRUCTION(test_gb, RA, RT, {
        state.r[1] = RA;
        run({ a.gb(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::gbb() {
    // Gather Bits from Bytes
    TEST_INSTRUCTION(test_gbb, RA, RT, {
        state.r[1] = RA;
        run({ a.gbb(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::gbh() {
    // Gather Bits from Halfwords
    TEST_INSTRUCTION(test_gbh, RA, RT, {
        state.r[1] = RA;
        run({ a.gbh(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::mpy() {
    // Multiply
    TEST_INSTRUCTION(test_mpy, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpy(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpya() {
    // Multiply and Add
    TEST_INSTRUCTION(test_mpya, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.mpya(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyh() {
    // Multiply High
    TEST_INSTRUCTION(test_mpyh, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyh(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyhh() {
    // Multiply High High
    TEST_INSTRUCTION(test_mpyhh, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyhh(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyhha() {
    // Multiply High High and Add
    TEST_INSTRUCTION(test_mpyhha, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyhha(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyhhau() {
    // Multiply High High Unsigned and Add
    TEST_INSTRUCTION(test_mpyhhau, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyhhau(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyhhu() {
    // Multiply High High Unsigned
    TEST_INSTRUCTION(test_mpyhhu, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyhhu(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyi() {
    // Multiply Immediate
    TEST_INSTRUCTION(test_mpyi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.mpyi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::mpys() {
    // Multiply and Shift Right
    TEST_INSTRUCTION(test_mpys, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpys(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyu() {
    // Multiply Unsigned
    TEST_INSTRUCTION(test_mpyu, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.mpyu(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::mpyui() {
    // Multiply Unsigned Immediate
    TEST_INSTRUCTION(test_mpyui, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.mpyui(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::nand() {
    // Nand
    TEST_INSTRUCTION(test_nand, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.nand(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::nor() {
    // Nor
    TEST_INSTRUCTION(test_nor, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.nor(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::or_() {
    // Or
    TEST_INSTRUCTION(test_or, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.or_(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::orbi() {
    // Or Byte Immediate
    TEST_INSTRUCTION(test_orbi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.orbi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::orc() {
    // Or with Complement
    TEST_INSTRUCTION(test_orc, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.orc(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::orhi() {
    // Or Halfword Immediate
    TEST_INSTRUCTION(test_orhi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.orhi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::ori() {
    // Or Word Immediate
    TEST_INSTRUCTION(test_ori, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.ori(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::orx() {
    // Or Across
    TEST_INSTRUCTION(test_orx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.orx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::selb() {
    // Select Bits
    TEST_INSTRUCTION(test_selb, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.selb(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    // TODO
}

void SPUTestRunner::sf() {
    // Subtract from Word
    TEST_INSTRUCTION(test_sf, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.sf(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::sfh() {
    // Subtract from Halfword
    TEST_INSTRUCTION(test_sfh, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.sfh(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::sfhi() {
    // Subtract from Halfword Immediate
    TEST_INSTRUCTION(test_sfhi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.sfhi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::sfi() {
    // Subtract from Word Immediate
    TEST_INSTRUCTION(test_sfi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.sfi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::sfx() {
    // Subtract from Extended
    TEST_INSTRUCTION(test_sfx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.sfx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shufb() {
    // Shuffle Bytes
    TEST_INSTRUCTION(test_shufb, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.shufb(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    test_shufb(
        V128::from_u32(0xDEADBEEF, 0xCAFEBABE, 0x1337C0DE, 0x0BADF00D),
        V128::from_u32(0x0DEFACED, 0x1337BABE, 0xDEAFFEED, 0xCACAFACE),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0xDEADBEEF, 0xCAFEBABE, 0x1337C0DE, 0x0BADF00D));
    test_shufb(
        V128::from_u32(0xDEADBEEF, 0xCAFEBABE, 0x1337C0DE, 0x0BADF00D),
        V128::from_u32(0x0DEFACED, 0x1337BABE, 0xDEAFFEED, 0xCACAFACE),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x0DEFACED, 0x1337BABE, 0xDEAFFEED, 0xCACAFACE));
    test_shufb(
        V128::from_u32(0xDEADBEEF, 0xCAFEBABE, 0x1337C0DE, 0x0BADF00D),
        V128::from_u32(0x0DEFACED, 0x1337BABE, 0xDEAFFEED, 0xCACAFACE),
        V128::from_u32(0x04050607, 0x11011202, 0x08071615, 0x1C1D1E1F),
        V128::from_u32(0xCAFEBABE, 0xEFADACBE, 0x13BEBA37, 0xCACAFACE));
}

void SPUTestRunner::sumb() {
    // Sum Bytes into Halfwords
    TEST_INSTRUCTION(test_sumb, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.sumb(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::xor_() {
    // Exclusive Or 
    TEST_INSTRUCTION(test_xor, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.xor_(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::xorbi() {
    // Exclusive Or Byte Immediate
    TEST_INSTRUCTION(test_xorbi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.xorbi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::xorhi() {
    // Exclusive Or Halfword Immediate
    TEST_INSTRUCTION(test_xorhi, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.xorhi(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::xori() {
    // Exclusive Or Word Immediate
    TEST_INSTRUCTION(test_xori, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.xori(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::xsbh() {
    // Extend Sign Byte to Halfword
    TEST_INSTRUCTION(test_xsbh, RA, RT, {
        state.r[1] = RA;
        run({ a.xsbh(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::xshw() {
    // Extend Sign Halfword to Word
    TEST_INSTRUCTION(test_xshw, RA, RT, {
        state.r[1] = RA;
        run({ a.xshw(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::xswd() {
    // Extend Sign Word to Doubleword
    TEST_INSTRUCTION(test_xswd, RA, RT, {
        state.r[1] = RA;
        run({ a.xswd(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}


// Shift and Rotate Instructions (Chapter 6)
void SPUTestRunner::shl() {
    // Shift Left Word
    TEST_INSTRUCTION(test_shl, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.shl(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shlh() {
    // Shift Left Halfword
    TEST_INSTRUCTION(test_shlh, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.shlh(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shlhi() {
    // Shift Left Halfword Immediate
    TEST_INSTRUCTION(test_shlhi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.shlhi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::shli() {
    // Shift Left Word Immediate
    TEST_INSTRUCTION(test_shli, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.shli(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::shlqbi() {
    // Shift Left Quadword by Bits
    TEST_INSTRUCTION(test_shlqbi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.shlqbi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shlqbii() {
    // Shift Left Quadword by Bits Immediate
    TEST_INSTRUCTION(test_shlqbii, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.shlqbii(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::shlqby() {
    // Shift Left Quadword by Bytes
    TEST_INSTRUCTION(test_shlqby, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.shlqby(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shlqbybi() {
    // Shift Left Quadword by Bytes from Bit Shift Count
    TEST_INSTRUCTION(test_shlqbybi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.shlqbybi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::shlqbyi() {
    // Shift Left Quadword by Bytes Immediate
    TEST_INSTRUCTION(test_shlqbyi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.shlqbyi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rot() {
    // Rotate Word
    TEST_INSTRUCTION(test_rot, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rot(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::roth() {
    // Rotate Halfword
    TEST_INSTRUCTION(test_roth, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.roth(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rothi() {
    // Rotate Halfword Immediate
    TEST_INSTRUCTION(test_rothi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rothi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rothm() {
    // Rotate and Mask Halfword
    TEST_INSTRUCTION(test_rothm, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rothm(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rothmi() {
    // Rotate and Mask Halfword Immediate
    TEST_INSTRUCTION(test_rothmi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rothmi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::roti() {
    // Rotate Word Immediate
    TEST_INSTRUCTION(test_roti, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.roti(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotm() {
    // Rotate and Mask Word
    TEST_INSTRUCTION(test_rotm, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotm(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotma() {
    // Rotate and Mask Algebraic Word
    TEST_INSTRUCTION(test_rotma, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotma(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotmah() {
    // Rotate and Mask Algebraic Halfword
    TEST_INSTRUCTION(test_rotmah, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotmah(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotmahi() {
    // Rotate and Mask Algebraic Halfword Immediate
    TEST_INSTRUCTION(test_rotmahi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotmahi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotmai() {
    // Rotate and Mask Algebraic Word Immediate
    TEST_INSTRUCTION(test_rotmai, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotmai(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotmi() {
    // Rotate and Mask Word Immediate
    TEST_INSTRUCTION(test_rotmi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotmi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqbi() {
    // Rotate Quadword by Bits
    TEST_INSTRUCTION(test_rotqbi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqbi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqbii() {
    // Rotate Quadword by Bits Immediate
    TEST_INSTRUCTION(test_rotqbii, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotqbii(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqby() {
    // Rotate Quadword by Bytes
    TEST_INSTRUCTION(test_rotqby, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqby(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqbybi() {
    // Rotate Quadword by Bytes from Bit Shift Count
    TEST_INSTRUCTION(test_rotqbybi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqbybi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqbyi() {
    // Rotate Quadword by Bytes Immediate
    TEST_INSTRUCTION(test_rotqbyi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotqbyi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqmbi() {
    // Rotate and Mask Quadword by Bits
    TEST_INSTRUCTION(test_rotqmbi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqmbi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqmbii() {
    // Rotate and Mask Quadword by Bits Immediate
    TEST_INSTRUCTION(test_rotqmbii, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotqmbii(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqmby() {
    // Rotate and Mask Quadword by Bytes
    TEST_INSTRUCTION(test_rotqmby, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqmby(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqmbybi() {
    // Rotate and Mask Quadword Bytes from Bit Shift Count
    TEST_INSTRUCTION(test_rotqmbybi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.rotqmbybi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::rotqmbyi() {
    // Rotate and Mask Quadword by Bytes Immediate
    TEST_INSTRUCTION(test_rotqmbyi, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.rotqmbyi(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}
