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
    // Floating Add (Double-Precision)
    TEST_INSTRUCTION(test_fadd, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fadd(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fadd(+2.0, +3.0, +5.0);
    test_fadd(+2.0, -3.0, -1.0);
    test_fadd( 0.0,  0.0,  0.0);
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
    // Floating Convert from Integer Double Word
    TEST_INSTRUCTION(test_fcfid, F1, F2, {
        (U64&)state.f[1] = F1;
        run({ a.fcfid(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fcfid(0xFFFFFFFF'FFFFFFFF,                   -1.0);
    test_fcfid(0x00000000'00000000,                    0.0);
    test_fcfid(0x00000000'00000010,                   16.0);
    test_fcfid(0x00000000'80000001,          +2147483649.0);
    test_fcfid(0xFFFFFFFF'7FFFFFFF,          -2147483649.0);
    test_fcfid(0x7FFFFFFF'FFFFFFFF, +9223372036854775807.0); // TODO: Why doesn't this work?
    test_fcfid(0x80000000'00000000, -9223372036854775808.0); // TODO: Why doesn't this work?
}

void PPCTestRunner::fcmpo() {
}

void PPCTestRunner::fcmpu() {
    // Floating Compare Unordered
    TEST_INSTRUCTION(test_fcmpu, CRFD, F1, F2, LT, GT, EQ, SO, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fcmpu(CRFD, f1, f2); });
        expect(state.cr.field[CRFD].lt == LT);
        expect(state.cr.field[CRFD].gt == GT);
        expect(state.cr.field[CRFD].eq == EQ);
        expect(state.cr.field[CRFD].so == SO);
    });

    test_fcmpu(cr0,  1.0,  2.0,  1,0,0,0);
    test_fcmpu(cr0,  2.5,  2.4,  0,1,0,0);
    test_fcmpu(cr1, -9.0,  0.0,  1,0,0,0);
    test_fcmpu(cr7, -7.0, -7.0,  0,0,1,0);
    test_fcmpu(cr7,  0.0,  0.0,  0,0,1,0);
}

void PPCTestRunner::fctidx() {
    // Floating Convert to Integer Double Word
    TEST_INSTRUCTION(test_fctid, F1, F2, {
        state.f[1] = F1;
        run({ a.fctid(f2, f1); });
        expect((U64&)state.f[2] == F2);
        // TODO: Other registers
    });

    // Rounding mode: Round toward zero
    test_fctid(                  -1.5, 0xFFFFFFFF'FFFFFFFF);
    test_fctid(                   0.0, 0x00000000'00000000);
    test_fctid(                   0.1, 0x00000000'00000000);
    test_fctid(                  16.9, 0x00000000'00000010);
    test_fctid(         +2147483649.0, 0x00000000'80000001);
    test_fctid(         -2147483649.0, 0xFFFFFFFF'7FFFFFFF);
    //test_fctid(+*223372036854775807.0, 0x7FFFFFFF'FFFFFFFF); // TODO: Why doesn't this work?
    //test_fctid(-9223372036854775808.0, 0x80000000'00000000); // TODO: Why doesn't this work?
    //test_fctiw(+9223372036854775808.0, 0x7FFFFFFF'FFFFFFFF); // TODO: Saturation check
    //test_fctiw(-9223372036854775809.0, 0x80000000'00000000); // TODO: Saturation check

    // TODO: Other rounding modes
}

void PPCTestRunner::fctidzx() {
    // Floating Convert to Integer Double Word with Round toward Zero
    TEST_INSTRUCTION(test_fctidz, F1, F2, {
        state.f[1] = F1;
        run({ a.fctidz(f2, f1); });
        expect((U64&)state.f[2] == F2);
        // TODO: Other registers
    });

    test_fctidz(                  -1.5, 0xFFFFFFFF'FFFFFFFF);
    test_fctidz(                   0.0, 0x00000000'00000000);
    test_fctidz(                   0.1, 0x00000000'00000000);
    test_fctidz(                  16.9, 0x00000000'00000010);
    test_fctidz(         +2147483649.0, 0x00000000'80000001);
    test_fctidz(         -2147483649.0, 0xFFFFFFFF'7FFFFFFF);
    //test_fctidz(+*223372036854775807.0, 0x7FFFFFFF'FFFFFFFF); // TODO: Why doesn't this work?
    //test_fctidz(-9223372036854775808.0, 0x80000000'00000000); // TODO: Why doesn't this work?
    //test_fctiwz(+9223372036854775808.0, 0x7FFFFFFF'FFFFFFFF); // TODO: Saturation check
    //test_fctiwz(-9223372036854775809.0, 0x80000000'00000000); // TODO: Saturation check
}

void PPCTestRunner::fctiwx() {
    // Floating Convert to Integer Word 
    TEST_INSTRUCTION(test_fctiw, F1, F2, {
        state.f[1] = F1;
        run({ a.fctiw(f2, f1); });
        expect(U32((U64&)state.f[2]) == F2);
        // TODO: Other registers
    });

    // Rounding mode: Round toward zero
    test_fctiw(         -1.5, 0xFFFFFFFF);
    test_fctiw(          0.0, 0x00000000);
    test_fctiw(          0.1, 0x00000000);
    test_fctiw(         16.9, 0x00000010);
    test_fctiw(+2147483647.0, 0x7FFFFFFF);
    test_fctiw(-2147483648.0, 0x80000000);
    //test_fctiw(+2147483648.0, 0x7FFFFFFF); // TODO: Saturation check
    //test_fctiw(-2147483649.0, 0x80000000); // TODO: Saturation check

    // TODO: Other rounding modes
}

void PPCTestRunner::fctiwzx() {
    // Floating Convert to Integer Word with Round toward Zero
    TEST_INSTRUCTION(test_fctiwz, F1, F2, {
        state.f[1] = F1;
        run({ a.fctiwz(f2, f1); });
        expect(U32((U64&)state.f[2]) == F2);
        // TODO: Other registers
    });

    test_fctiwz(         -1.5, 0xFFFFFFFF);
    test_fctiwz(          0.0, 0x00000000);
    test_fctiwz(          0.1, 0x00000000);
    test_fctiwz(         16.9, 0x00000010);
    test_fctiwz(+2147483647.0, 0x7FFFFFFF);
    test_fctiwz(-2147483648.0, 0x80000000);
    //test_fctiwz(+2147483648.0, 0x7FFFFFFF); // TODO: Saturation check
    //test_fctiwz(-2147483649.0, 0x80000000); // TODO: Saturation check
}

void PPCTestRunner::fdivx() {
    // Floating Divide (Double-Precision)
    TEST_INSTRUCTION(test_fdiv, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fdiv(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fdiv( 6.0,  3.0,  2.0);
    test_fdiv( 1.0, -0.5, -2.0);
    test_fdiv(-0.8,  2.0, -0.4);
    test_fdiv( 0.0,  5.0,  0.0);
    test_fdiv(-3.0, -3.0,  1.0);
}

void PPCTestRunner::fdivsx() {
    // Floating Divide Single
    TEST_INSTRUCTION(test_fdivs, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fdivs(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fdivs( 6.0f,  3.0f,  2.0f);
    test_fdivs( 1.0f, -0.5f, -2.0f);
    test_fdivs(-0.8f,  2.0f, -0.4f);
    test_fdivs( 0.0f,  5.0f,  0.0f);
    test_fdivs(-3.0f, -3.0f,  1.0f);
}

void PPCTestRunner::fmaddx() {
    // Floating Multiply-Add (Double-Precision)
    TEST_INSTRUCTION(test_fmadd, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fmadd(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fmadd( 5.0,  5.0,  0.0, 25.0);
    test_fmadd( 0.0,  7.0, -3.0, -3.0);
    test_fmadd( 3.0, -2.0,  4.0, -2.0);
}

void PPCTestRunner::fmaddsx() {
    // Floating Multiply-Add Single
    TEST_INSTRUCTION(test_fmadds, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fmadds(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fmadds( 5.0f,  5.0f,  0.0f, 25.0f);
    test_fmadds( 0.0f,  7.0f, -3.0f, -3.0f);
    test_fmadds( 3.0f, -2.0f,  4.0f, -2.0f);
}

void PPCTestRunner::fmrx() {
    // Floating Move Register
    TEST_INSTRUCTION(test_fmr, F1, F2, {
        state.f[1] = F1;
        run({ a.fmr(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fmr(+3.0, +3.0);
    test_fmr( 0.0,  0.0);
    test_fmr(-0.0, -0.0);
    test_fmr(-3.0, -3.0);
}

void PPCTestRunner::fmsubx() {
    // Floating Multiply-Subtract (Double-Precision)
    TEST_INSTRUCTION(test_fmsub, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fmsub(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fmsub( 5.0,  5.0,  0.0, 25.0);
    test_fmsub( 0.0,  7.0,  3.0, -3.0);
    test_fmsub( 3.0, -2.0, -4.0, -2.0);
}

void PPCTestRunner::fmsubsx() {
    // Floating Multiply-Subtract Single
    TEST_INSTRUCTION(test_fmsubs, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fmsubs(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fmsubs( 5.0f,  5.0f,  0.0f, 25.0f);
    test_fmsubs( 0.0f,  7.0f,  3.0f, -3.0f);
    test_fmsubs( 3.0f, -2.0f, -4.0f, -2.0f);
}

void PPCTestRunner::fmulx() {
    // Floating Multiply (Double-Precision)
    TEST_INSTRUCTION(test_fmul, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fmul(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fmul( 5.0,  5.0, 25.0);
    test_fmul(-3.0, -4.0, 12.0);
    test_fmul( 0.0,  7.0,  0.0);
    test_fmul(-2.0,  0.0,  0.0);
    test_fmul(-2.0,  3.0, -6.0);
}

void PPCTestRunner::fmulsx() {
    // Floating Multiply Single
    TEST_INSTRUCTION(test_fmuls, F1, F2, F3, {
        state.f[1] = F1;
        state.f[2] = F2;
        run({ a.fmuls(f3, f1, f2); });
        expect(state.f[3] == F3);
        // TODO: Other registers
    });

    test_fmuls( 5.0f,  5.0f, 25.0f);
    test_fmuls(-3.0f, -4.0f, 12.0f);
    test_fmuls( 0.0f,  7.0f,  0.0f);
    test_fmuls(-2.0f,  0.0f,  0.0f);
    test_fmuls(-2.0f,  3.0f, -6.0f);
}

void PPCTestRunner::fnabsx() {
    // Floating Negative Absolute Value
    TEST_INSTRUCTION(test_fnabs, F1, F2, {
        state.f[1] = F1;
        run({ a.fnabs(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fnabs(+3.5, -3.5);
    test_fnabs(-3.0, -3.0);
}

void PPCTestRunner::fnegx() {
    // Floating Negate
    TEST_INSTRUCTION(test_fneg, F1, F2, {
        state.f[1] = F1;
        run({ a.fneg(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fneg(+3.0, -3.0);
    test_fneg(+0.0, -0.0);
    test_fneg(-0.0, +0.0);
    test_fneg(-3.5, +3.5);
}

void PPCTestRunner::fnmaddx() {
    // Floating Negative Multiply-Add (Double-Precision)
    TEST_INSTRUCTION(test_fnmadd, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fnmadd(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fnmadd(-5.0,  5.0,  0.0, 25.0);
    test_fnmadd( 2.0,  3.0,  0.0, -6.0);
    test_fnmadd( 1.0,  1.0, -1.0,  0.0);
    test_fnmadd( 0.0,  7.0, -3.0,  3.0);
    test_fnmadd( 3.0, -2.0,  4.0,  2.0);
}

void PPCTestRunner::fnmaddsx() {
    // Floating Negative Multiply-Add Single
    TEST_INSTRUCTION(test_fnmadds, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fnmadds(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fnmadds(-5.0f,  5.0f,  0.0f, 25.0f);
    test_fnmadds( 2.0f,  3.0f,  0.0f, -6.0f);
    test_fnmadds( 1.0f,  1.0f, -1.0f,  0.0f);
    test_fnmadds( 0.0f,  7.0f, -3.0f,  3.0f);
    test_fnmadds( 3.0f, -2.0f,  4.0f,  2.0f);
}

void PPCTestRunner::fnmsubx() {
    // Floating Negative Multiply-Subtract (Double-Precision) 
    TEST_INSTRUCTION(test_fnmsub, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fnmsub(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fnmsub(-5.0,  5.0,  0.0, 25.0);
    test_fnmsub( 2.0,  3.0,  0.0, -6.0);
    test_fnmsub( 1.0,  1.0,  1.0,  0.0);
    test_fnmsub( 0.0,  7.0,  3.5,  3.5);
    test_fnmsub( 3.0, -2.0, -4.0,  2.0);
}

void PPCTestRunner::fnmsubsx() {
    // Floating Negative Multiply-Subtract Single
    TEST_INSTRUCTION(test_fnmsubs, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fnmsubs(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fnmsubs(-5.0f,  5.0f,  0.0f, 25.0f);
    test_fnmsubs( 2.0f,  3.0f,  0.0f, -6.0f);
    test_fnmsubs( 1.0f,  1.0f,  1.0f,  0.0f);
    test_fnmsubs( 0.0f,  7.0f,  3.5f,  3.5f);
    test_fnmsubs( 3.0f, -2.0f, -4.0f,  2.0f);
}

void PPCTestRunner::fresx() {
}

void PPCTestRunner::frspx() {
}

void PPCTestRunner::frsqrtex() {
    // Floating Reciprocal Square Root Estimate
    TEST_INSTRUCTION(test_frsqrte, F1, F2, {
        state.f[1] = F1;
        run({ a.frsqrte(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_frsqrte( 1.0, 0.99975585937500000);
    test_frsqrte(64.0, 0.12496948242187500);
    test_frsqrte( 0.5, 1.41381835937500000);
}

void PPCTestRunner::fselx() {
    // Floating Select
    TEST_INSTRUCTION(test_fsel, F1, F2, F3, F4, {
        state.f[1] = F1;
        state.f[2] = F2;
        state.f[3] = F3;
        run({ a.fsel(f4, f1, f2, f3); });
        expect(state.f[4] == F4);
        // TODO: Other registers
    });

    test_fsel(+2.0,  3.0,  4.0,  3.0);
    test_fsel(-2.0,  3.0, -4.0, -4.0);
    test_fsel( 0.0, -3.0,  4.0, -3.0);
}

void PPCTestRunner::fsqrtx() {
    // Floating Absolute Value (Double-Precision) 
    TEST_INSTRUCTION(test_fsqrt, F1, F2, {
        state.f[1] = F1;
        run({ a.fsqrt(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fsqrt( 1.0, 1.0);
    test_fsqrt(64.0, 8.0);
    test_fsqrt( 0.5, 0.70710678118654757);
}

void PPCTestRunner::fsqrtsx() {
    // Floating Absolute Value Single
    TEST_INSTRUCTION(test_fsqrts, F1, F2, {
        state.f[1] = F1;
        run({ a.fsqrts(f2, f1); });
        expect(state.f[2] == F2);
        // TODO: Other registers
    });

    test_fsqrts( 1.0f, 1.0f);
    test_fsqrts(64.0f, 8.0f);
    test_fsqrts( 0.5f, 0.70710678118654757f);
}

void PPCTestRunner::fsubx() {
    // Floating Substract (Double-Precision)
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
