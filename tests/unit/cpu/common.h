/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/macro.h"

// Common
#define TEST_INSTRUCTION_INIT() \
    memset(&state, 0, sizeof(state))

// Set a lambda with auto arguments
#define TEST_INSTRUCTION(...) \
    __NUCLEUS_MACRO_DISPATCH(TEST_INSTRUCTION, __VA_ARGS__)(__VA_ARGS__)

#define TEST_INSTRUCTION2(name, func) \
    auto name = [&]() { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION3(name, a0, func) \
    auto name = [&](auto a0) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION4(name, a0, a1, func) \
    auto name = [&](auto a0, auto a1) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION5(name, a0, a1, a2, func) \
    auto name = [&](auto a0, auto a1, auto a2) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION6(name, a0, a1, a2, a3, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION7(name, a0, a1, a2, a3, a4, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION8(name, a0, a1, a2, a3, a4, a5, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION9(name, a0, a1, a2, a3, a4, a5, a6, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION10(name, a0, a1, a2, a3, a4, a5, a6, a7, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION11(name, a0, a1, a2, a3, a4, a5, a6, a7, a8, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7, auto a8) { \
        TEST_INSTRUCTION_INIT(); func \
    }
#define TEST_INSTRUCTION12(name, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7, auto a8, auto a9) { \
        TEST_INSTRUCTION_INIT(); func \
    }
