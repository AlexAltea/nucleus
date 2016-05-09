/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_ppc.h"

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void PPCTestRunner::fabsx() {
    // Floating Absolute Value 
    TEST_INSTRUCTION(test_fabs, F1, F2, {
        state.f[1] = F1;
        run({ a.fabs(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fabs(+3.0, +3.0);
    test_fabs(-3.0, +3.0);
}

void PPCTestRunner::faddx() {
    // Floating Add (Double Precision)
    TEST_INSTRUCTION(test_fadd, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fadd(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fadd(+2.0, +3.0, +5.0);
    test_fadd(+2.0, -3.0, -1.0);
}

void PPCTestRunner::faddsx() {
    // Floating Add Single
    TEST_INSTRUCTION(test_fadds, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fadds(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fadds(+2.0f, +3.0f, +5.0f);
    test_fadds(+2.0f, -3.0f, -1.0f);
}

void PPCTestRunner::fcfidx() {
}

void PPCTestRunner::fcmpo() {
}

void PPCTestRunner::fcmpu() {
}

void PPCTestRunner::fctidx() {
}

void PPCTestRunner::fctidzx() {
}

void PPCTestRunner::fctiwx() {
}

void PPCTestRunner::fctiwzx() {
}

void PPCTestRunner::fdivx() {
}

void PPCTestRunner::fdivsx() {
}

void PPCTestRunner::fmaddx() {
}

void PPCTestRunner::fmaddsx() {
}

void PPCTestRunner::fmrx() {
}

void PPCTestRunner::fmsubx() {
}

void PPCTestRunner::fmsubsx() {
}

void PPCTestRunner::fmulx() {
}

void PPCTestRunner::fmulsx() {
}

void PPCTestRunner::fnabsx() {
}

void PPCTestRunner::fnegx() {
}

void PPCTestRunner::fnmaddx() {
}

void PPCTestRunner::fnmaddsx() {
}

void PPCTestRunner::fnmsubx() {
}

void PPCTestRunner::fnmsubsx() {
}

void PPCTestRunner::fresx() {
}

void PPCTestRunner::frspx() {
}

void PPCTestRunner::frsqrtex() {
}

void PPCTestRunner::fselx() {
}

void PPCTestRunner::fsqrtx() {
}

void PPCTestRunner::fsqrtsx() {
}

void PPCTestRunner::fsubx() {
    // Floating Substract (Double Precision)
    TEST_INSTRUCTION(test_fsub, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fsub(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fsub(+2.0, +3.0, -1.0);
    test_fsub(+2.0, -3.0, +5.0);
}

void PPCTestRunner::fsubsx() {
    // Floating Substract Single
    TEST_INSTRUCTION(test_fsubs, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fsubs(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fsubs(+2.0f, +3.0f, -1.0f);
    test_fsubs(+2.0f, -3.0f, +5.0f);
}

void PPCTestRunner::mcrfs() {
}

void PPCTestRunner::mffsx() {
}

void PPCTestRunner::mtfsb0x() {
}

void PPCTestRunner::mtfsb1x() {
}

void PPCTestRunner::mtfsfix() {
}

void PPCTestRunner::mtfsfx() {
}
