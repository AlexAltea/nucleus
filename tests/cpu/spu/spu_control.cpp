/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_spu.h"

/**
 * SPU Instructions:
 *  - Control Instructions (Chapter 10)
 */

// Control Instructions (Chapter 10)
void SPUTestRunner::dsync() {
    // Synchronize Data
}

void SPUTestRunner::lnop() {
    // No Operation (Load)
}

void SPUTestRunner::mfspr() {
    // Move from Special-Purpose Register
}

void SPUTestRunner::mtspr() {
    // Move to Special-Purpose Register
}

void SPUTestRunner::nop() {
    // No Operation (Execute)
}

void SPUTestRunner::stop() {
    // Stop and Signal
}

void SPUTestRunner::stopd() {
    // Stop and Signal with Dependencies
}

void SPUTestRunner::sync() {
    // Synchronize
}
