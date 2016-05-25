/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_spu.h"

/**
 * SPU Instructions:
 *  - Memory-Load/Store Instructions (Chapter 3)
 */

// Memory-Load/Store Instructions (Chapter 3)
void SPUTestRunner::cbd() {
    // Generate Controls for Byte Insertion (d-form)
    TEST_INSTRUCTION(test_cbd, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.cbd(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::cbx() {
    // Generate Controls for Byte Insertion (x-form)
    TEST_INSTRUCTION(test_cbx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.cbx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::cdd() {
    // Generate Controls for Doubleword Insertion (d-form)
    TEST_INSTRUCTION(test_cdd, I7, RA_ps, RT, {
        state.r[1].u32[3] = RA_ps;
        run({ a.cdd(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO: Are these tests correct?
    test_cdd(0, 0x00000000, V128::from_u32(0x00010203, 0x04050607, 0x18191A1B, 0x1C1D1E1F));
    test_cdd(8, 0x00000000, V128::from_u32(0x10111213, 0x14151617, 0x00010203, 0x04050607));
    test_cdd(0, 0x00000008, V128::from_u32(0x10111213, 0x14151617, 0x00010203, 0x04050607));
    test_cdd(8, 0x00000008, V128::from_u32(0x00010203, 0x04050607, 0x18191A1B, 0x1C1D1E1F));
}

void SPUTestRunner::cdx() {
    // Generate Controls for Doubleword Insertion (x-form)
    TEST_INSTRUCTION(test_cbx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.cbx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::chd() {
    // Generate Controls for Halfword Insertion (d-form)
    TEST_INSTRUCTION(test_chd, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.chd(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::chx() {
    //  Generate Controls for Halfword Insertion (x-form)
    TEST_INSTRUCTION(test_chx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.chx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::cwd() {
    // Generate Controls for Word Insertion (d-form)
    TEST_INSTRUCTION(test_cwd, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.cwd(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::cwx() {
    // Generate Controls for Word Insertion (x-form)
    TEST_INSTRUCTION(test_cwx, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.cwx(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::lqa() {
    // Load Quadword (a-form)
    TEST_INSTRUCTION(test_lqa, I16, RT, {
        run({ a.lqa(r1, I16); });
        expect(state.r[1] == RT);
    });

    // TODO
}

void SPUTestRunner::lqd() {
    // Load Quadword (d-form)
    TEST_INSTRUCTION(test_lqd, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.lqd(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::lqr() {
    // Load Quadword Instruction Relative (a-form)
    TEST_INSTRUCTION(test_lqr, I16, RT, {
        run({ a.lqr(r1, I16); });
        expect(state.r[1] == RT);
    });

    // TODO
}

void SPUTestRunner::lqx() {
    // Load Quadword (x-form)
    TEST_INSTRUCTION(test_lqx, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.lqx(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::stqa() {
    // Store Quadword (a-form)
    TEST_INSTRUCTION(test_stqa, I16, RT, {
        run({ a.stqa(r1, I16); });
        expect(state.r[1] == RT);
    });

    // TODO
}

void SPUTestRunner::stqd() {
    // Store Quadword (d-form)
    TEST_INSTRUCTION(test_stqd, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.stqd(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::stqr() {
    // Store Quadword Instruction Relative (a-form)
    TEST_INSTRUCTION(test_stqr, I16, RT, {
        run({ a.stqr(r1, I16); });
        expect(state.r[1] == RT);
    });

    // TODO
}

void SPUTestRunner::stqx() {
    // Store Quadword (x-form)
    TEST_INSTRUCTION(test_stqx, I10, RA, RT, {
        state.r[1] = RA;
        run({ a.stqx(r2, r1, I10); });
        expect(state.r[2] == RT);
    });

    // TODO
}
