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
}

void SPUTestRunner::absdb() {
}

void SPUTestRunner::addx() {
}

void SPUTestRunner::ah() {
}

void SPUTestRunner::ahi() {
}

void SPUTestRunner::ai() {
}

void SPUTestRunner::and_() {
}

void SPUTestRunner::andc() {
}

void SPUTestRunner::andbi() {
}

void SPUTestRunner::andhi() {
}

void SPUTestRunner::andi() {
}

void SPUTestRunner::avgb() {
}

void SPUTestRunner::bg() {
}

void SPUTestRunner::bgx() {
}

void SPUTestRunner::cg() {
}

void SPUTestRunner::cgx() {
}

void SPUTestRunner::clz() {
}

void SPUTestRunner::cntb() {
}

void SPUTestRunner::eqv() {
}

void SPUTestRunner::fsm() {
}

void SPUTestRunner::fsmb() {
}

void SPUTestRunner::fsmh() {
}

void SPUTestRunner::gb() {
}

void SPUTestRunner::gbb() {
}

void SPUTestRunner::gbh() {
}

void SPUTestRunner::mpy() {
}

void SPUTestRunner::mpya() {
}

void SPUTestRunner::mpyh() {
}

void SPUTestRunner::mpyhh() {
}

void SPUTestRunner::mpyhha() {
}

void SPUTestRunner::mpyhhau() {
}

void SPUTestRunner::mpyhhu() {
}

void SPUTestRunner::mpyi() {
}

void SPUTestRunner::mpys() {
}

void SPUTestRunner::mpyu() {
}

void SPUTestRunner::mpyui() {
}

void SPUTestRunner::nand() {
}

void SPUTestRunner::nor() {
}

void SPUTestRunner::or_() {
}

void SPUTestRunner::orbi() {
}

void SPUTestRunner::orc() {
}

void SPUTestRunner::orhi() {
}

void SPUTestRunner::ori() {
}

void SPUTestRunner::orx() {
}

void SPUTestRunner::selb() {
}

void SPUTestRunner::sf() {
}

void SPUTestRunner::sfh() {
}

void SPUTestRunner::sfhi() {
}

void SPUTestRunner::sfi() {
}

void SPUTestRunner::sfx() {
}

void SPUTestRunner::shufb() {
}

void SPUTestRunner::sumb() {
}

void SPUTestRunner::xor_() {
}

void SPUTestRunner::xorbi() {
}

void SPUTestRunner::xorhi() {
}

void SPUTestRunner::xori() {
}

void SPUTestRunner::xsbh() {
}

void SPUTestRunner::xshw() {
}

void SPUTestRunner::xswd() {
}


// Shift and Rotate Instructions (Chapter 6)
void SPUTestRunner::shl() {
}

void SPUTestRunner::shlh() {
}

void SPUTestRunner::shlhi() {
}

void SPUTestRunner::shli() {
}

void SPUTestRunner::shlqbi() {
}

void SPUTestRunner::shlqbii() {
}

void SPUTestRunner::shlqby() {
}

void SPUTestRunner::shlqbybi() {
}

void SPUTestRunner::shlqbyi() {
}

void SPUTestRunner::rot() {
}

void SPUTestRunner::roth() {
}

void SPUTestRunner::rothi() {
}

void SPUTestRunner::rothm() {
}

void SPUTestRunner::rothmi() {
}

void SPUTestRunner::roti() {
}

void SPUTestRunner::rotm() {
}

void SPUTestRunner::rotma() {
}

void SPUTestRunner::rotmah() {
}

void SPUTestRunner::rotmahi() {
}

void SPUTestRunner::rotmai() {
}

void SPUTestRunner::rotmi() {
}

void SPUTestRunner::rotqbi() {
}

void SPUTestRunner::rotqbii() {
}

void SPUTestRunner::rotqby() {
}

void SPUTestRunner::rotqbybi() {
}

void SPUTestRunner::rotqbyi() {
}

void SPUTestRunner::rotqmbi() {
}

void SPUTestRunner::rotqmbii() {
}

void SPUTestRunner::rotqmby() {
}

void SPUTestRunner::rotqmbybi() {
}

void SPUTestRunner::rotqmbyi() {
}
