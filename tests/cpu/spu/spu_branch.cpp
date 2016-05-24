/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_spu.h"

/**
 * SPU Instructions:
 *  - Compare, Branch and Halt Instructions (Chapter 7)
 *  - Hint-for-Branch Instructions (Chapter 8)
 */

// Compare, Branch and Halt Instructions (Chapter 7)
void SPUTestRunner::bi() {
    // Branch Indirect
}

void SPUTestRunner::bihnz() {
    // Branch Indirect If Not Zero Halfword
}

void SPUTestRunner::bihz() {
    // Branch Indirect If Zero Halfword
}

void SPUTestRunner::binz() {
    // Branch Indirect If Not Zero
}

void SPUTestRunner::bisl() {
    // Branch Indirect and Set Link
}

void SPUTestRunner::bisled() {
    // Branch Indirect and Set Link if External Data
}

void SPUTestRunner::biz() {
    // Branch Indirect If Zero
}

void SPUTestRunner::br() {
    // Branch Relative
}

void SPUTestRunner::bra() {
    // Branch Absolute
}

void SPUTestRunner::brasl() {
    // Branch Absolute and Set Link
}

void SPUTestRunner::brhnz() {
    // Branch If Not Zero Halfword
}

void SPUTestRunner::brhz() {
    // Branch If Zero Halfword
}

void SPUTestRunner::brnz() {
    // Branch If Not Zero Word
}

void SPUTestRunner::brsl() {
    // Branch Relative and Set Link
}

void SPUTestRunner::brz() {
    // Branch If Zero Word
}

void SPUTestRunner::ceq() {
    // Compare Equal Word
}

void SPUTestRunner::ceqb() {
    // Compare Equal Byte
}

void SPUTestRunner::ceqbi() {
    // Compare Equal Byte Immediate
}

void SPUTestRunner::ceqh() {
    // Compare Equal Halfword
}

void SPUTestRunner::ceqhi() {
    // Compare Equal Halfword Immediate
}

void SPUTestRunner::ceqi() {
    // Compare Equal Word Immediate
}

void SPUTestRunner::cgt() {
    // Compare Greater Than Word
}

void SPUTestRunner::cgtb() {
    // Compare Greater Than Byte
}

void SPUTestRunner::cgtbi() {
    // Compare Greater Than Byte Immediate
}

void SPUTestRunner::cgth() {
    // Compare Greater Than Halfword
}

void SPUTestRunner::cgthi() {
    // Compare Greater Than Halfword Immediate
}

void SPUTestRunner::cgti() {
    // Compare Greater Than Word Immediate
}

void SPUTestRunner::clgt() {
    // Compare Logical Greater Than Word
}

void SPUTestRunner::clgtb() {
    // Compare Logical Greater Than Byte
}

void SPUTestRunner::clgtbi() {
    // Compare Logical Greater Than Byte Immediate
}

void SPUTestRunner::clgth() {
    // Compare Logical Greater Than Halfword
}

void SPUTestRunner::clgthi() {
    // Compare Logical Greater Than Halfword Immediate
}

void SPUTestRunner::clgti() {
    // Compare Logical Greater Than Word Immediate
}

void SPUTestRunner::heq() {
    // Halt If Equal
}

void SPUTestRunner::heqi() {
    // Halt If Equal Immediate
}

void SPUTestRunner::hgt() {
    // Halt If Greater Than
}

void SPUTestRunner::hgti() {
    // Halt If Greater Than Immediate
}

void SPUTestRunner::hlgt() {
    // Halt If Logically Greater Than
}

void SPUTestRunner::hlgti() {
    // Halt If Logically Greater Than Immediate
}

void SPUTestRunner::iret() {
    // Interrupt Return
}


// Hint-for-Branch Instructions (Chapter 8)
void SPUTestRunner::hbr() {
    // Hint for Branch (r-form)
}

void SPUTestRunner::hbra() {
    // Hint for Branch (a-form)
}

void SPUTestRunner::hbrr() {
    // Hint for Branch Relative
}
