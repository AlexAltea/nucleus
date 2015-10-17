/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/macro.h"

#define TEST_INSTRUCTION(...) \
    __NUCLEUS_MACRO_DISPATCH(TEST_INSTRUCTION, __VA_ARGS__)(__VA_ARGS__)

#define TEST_INSTRUCTION2(name, func) \
    auto name = [&]() func
#define TEST_INSTRUCTION3(name, a0, func) \
    auto name = [&](auto a0) func
#define TEST_INSTRUCTION4(name, a0, a1, func) \
    auto name = [&](auto a0, auto a1) func
#define TEST_INSTRUCTION5(name, a0, a1, a2, func) \
    auto name = [&](auto a0, auto a1, auto a2) func 
#define TEST_INSTRUCTION6(name, a0, a1, a2, a3, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3) func
#define TEST_INSTRUCTION7(name, a0, a1, a2, a3, a4, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4) func
#define TEST_INSTRUCTION8(name, a0, a1, a2, a3, a4, a5, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5) func
#define TEST_INSTRUCTION9(name, a0, a1, a2, a3, a4, a5, a6, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6) func
#define TEST_INSTRUCTION10(name, a0, a1, a2, a3, a4, a5, a6, a7, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7) func
