/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_spu.h"

/**
 * SPU Instructions:
 *  - Floating-Point Instructions (Chapter 9)
 */

// Floating-Point Instructions (Chapter 9)
void SPUTestRunner::cflts() {
    // Convert Floating to Signed Integer
    TEST_INSTRUCTION(test_cflts, I8, RA, RT, {
        state.r[1] = RA;
        run({ a.cflts(r2, r1, I8); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::cfltu() {
    // Convert Floating to Unsigned Integer
    TEST_INSTRUCTION(test_cfltu, I8, RA, RT, {
        state.r[1] = RA;
        run({ a.cfltu(r2, r1, I8); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::csflt() {
    // Convert Signed Integer to Floating
    TEST_INSTRUCTION(test_csflt, I8, RA, RT, {
        state.r[1] = RA;
        run({ a.csflt(r2, r1, I8); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::cuflt() {
    // Convert Unsigned Integer to Floating
    TEST_INSTRUCTION(test_cuflt, I8, RA, RT, {
        state.r[1] = RA;
        run({ a.cuflt(r2, r1, I8); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::dfa() {
    // Double Floating Add
    TEST_INSTRUCTION(test_dfa, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfa(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfceq() {
    // Double Floating Compare Equal
    TEST_INSTRUCTION(test_dfceq, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfceq(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfcgt() {
    // Double Floating Compare Greater Than
    TEST_INSTRUCTION(test_dfcgt, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfcgt(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfcmeq() {
    // Double Floating Compare Magnitude Equal
    TEST_INSTRUCTION(test_dfcmeq, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfcmeq(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfcmgt() {
    // Double Floating Compare Magnitude Greater Than
    TEST_INSTRUCTION(test_dfcmgt, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfcmgt(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfm() {
    // Double Floating Multiply
    TEST_INSTRUCTION(test_dfm, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfm(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfma() {
    // Double Floating Multiply and Add
    TEST_INSTRUCTION(test_dfma, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfma(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfms() {
    // Double Floating Multiply and Subtract
    TEST_INSTRUCTION(test_dfms, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfms(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfnma() {
    // Double Floating Negative Multiply and Add
    TEST_INSTRUCTION(test_dfnma, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfnma(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfnms() {
    // Double Floating Negative Multiply and Subtract
    TEST_INSTRUCTION(test_dfnms, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfnms(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dfs() {
    // Double Floating Subtract
    TEST_INSTRUCTION(test_dfs, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.dfs(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::dftsv() {
    // Double Floating Test Special Value
    TEST_INSTRUCTION(test_dftsv, I7, RA, RT, {
        state.r[1] = RA;
        run({ a.dftsv(r2, r1, I7); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::fa() {
    // Floating Add
    TEST_INSTRUCTION(test_fa, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fa(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fceq() {
    // Floating Compare Equal
    TEST_INSTRUCTION(test_fceq, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fceq(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fcgt() {
    // t Floating Compare Greater Than
    TEST_INSTRUCTION(test_fcgt, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fcgt(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fcmeq() {
    // Floating Compare Magnitude Equal
    TEST_INSTRUCTION(test_fcmeq, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fcmeq(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fcmgt() {
    // Floating Compare Magnitude Greater Than
    TEST_INSTRUCTION(test_fcmgt, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fcmgt(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fesd() {
    // Floating Extend Single to Double
    TEST_INSTRUCTION(test_fesd, RA, RT, {
        state.r[1] = RA;
        run({ a.fesd(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::fi() {
    // Floating Interpolate
    TEST_INSTRUCTION(test_fi, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fi(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fm() {
    // Floating Multiply
    TEST_INSTRUCTION(test_fm, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fm(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fma() {
    // Floating Multiply and Add
    TEST_INSTRUCTION(test_fma, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.fma(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    // TODO
}

void SPUTestRunner::fms() {
    // Floating Multiply and Subtract
    TEST_INSTRUCTION(test_fms, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.fms(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    // TODO
}

void SPUTestRunner::fnms() {
    // Floating Negative Multiply and Subtract
    TEST_INSTRUCTION(test_fnms, RA, RB, RC, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.r[3] = RC;
        run({ a.fnms(r4, r1, r2, r3); });
        expect(state.r[4] == RT);
    });

    // TODO
}

void SPUTestRunner::frds() {
    // Floating Round Double to Single
    TEST_INSTRUCTION(test_frds, RA, RT, {
        state.r[1] = RA;
        run({ a.frds(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::frest() {
    // Floating Round Double to Single
    TEST_INSTRUCTION(test_frest, RA, RT, {
        state.r[1] = RA;
        run({ a.frest(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::frsqest() {
    // Floating Round Double to Single
    TEST_INSTRUCTION(test_frsqest, RA, RT, {
        state.r[1] = RA;
        run({ a.frsqest(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}

void SPUTestRunner::fs() {
    // Floating Subtract
    TEST_INSTRUCTION(test_fs, RA, RB, RT, {
        state.r[1] = RA;
        state.r[2] = RB;
        run({ a.fs(r3, r1, r2); });
        expect(state.r[3] == RT);
    });

    // TODO
}

void SPUTestRunner::fscrrd() {
    // Floating-Point Status and Control Register Read
    TEST_INSTRUCTION(test_fscrrd, RT, {
        run({ a.fscrrd(r1); });
        expect(state.r[1] == RT);
    });

    // TODO
}

void SPUTestRunner::fscrwr() {
    // Floating-Point Status and Control Register Write
    TEST_INSTRUCTION(test_fscrwr, RA, RT, {
        state.r[1] = RA;
        run({ a.fscrwr(r2, r1); });
        expect(state.r[2] == RT);
    });

    // TODO
}
