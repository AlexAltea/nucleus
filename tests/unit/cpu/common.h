/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/macro.h"
#include "tests/unit/common.h"

// Set a lambda with auto arguments
#define TEST_INSTRUCTION(...) \
    __NUCLEUS_MACRO_DISPATCH(TEST_INSTRUCTION, __VA_ARGS__)(__VA_ARGS__)
#define TEST_INSTRUCTION2(name, func) \
    auto name = [&]() { func }
#define TEST_INSTRUCTION3(name, a0, func) \
    auto name = [&](auto a0) { func }
#define TEST_INSTRUCTION4(name, a0, a1, func) \
    auto name = [&](auto a0, auto a1) { func }
#define TEST_INSTRUCTION5(name, a0, a1, a2, func) \
    auto name = [&](auto a0, auto a1, auto a2) { func }
#define TEST_INSTRUCTION6(name, a0, a1, a2, a3, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3) { func }
#define TEST_INSTRUCTION7(name, a0, a1, a2, a3, a4, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4) { func }
#define TEST_INSTRUCTION8(name, a0, a1, a2, a3, a4, a5, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5) { func }
#define TEST_INSTRUCTION9(name, a0, a1, a2, a3, a4, a5, a6, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6) { func }
#define TEST_INSTRUCTION10(name, a0, a1, a2, a3, a4, a5, a6, a7, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7) { func }
#define TEST_INSTRUCTION11(name, a0, a1, a2, a3, a4, a5, a6, a7, a8, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7, auto a8) { func }
#define TEST_INSTRUCTION12(name, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, func) \
    auto name = [&](auto a0, auto a1, auto a2, auto a3, auto a4, auto a5, auto a6, auto a7, auto a8, auto a9) { func }

// Given
#define given(...) \
    __NUCLEUS_MACRO_DISPATCH(given, __VA_ARGS__)(__VA_ARGS__)
#define given1(a0) \
    TEST_INSTRUCTION_GIVEN(); a0;
#define given2(a0, a1) \
    TEST_INSTRUCTION_GIVEN(); a0; a1;
#define given3(a0, a1, a2) \
    TEST_INSTRUCTION_GIVEN(); a0; a1; a2;
#define given4(a0, a1, a2, a3) \
    TEST_INSTRUCTION_GIVEN(); a0; a1; a2; a3;
#define given5(a0, a1, a2, a3, a4) \
    TEST_INSTRUCTION_GIVEN(); a0; a1; a2; a3; a4;

// When
#define when(...) \
    __NUCLEUS_MACRO_DISPATCH(when, __VA_ARGS__)(__VA_ARGS__)
#define when1(a0) \
    TEST_INSTRUCTION_WHEN(a0)
#define when2(a0, a1) \
    TEST_INSTRUCTION_WHEN(a0; a1)
#define when3(a0, a1, a2) \
    TEST_INSTRUCTION_WHEN(a0; a1; a2)
#define when4(a0, a1, a2, a3) \
    TEST_INSTRUCTION_WHEN(a0; a1; a2; a3)
#define when5(a0, a1, a2, a3, a4) \
    TEST_INSTRUCTION_WHEN(a0; a1; a2; a3; a4)

// Then
#define then(...) \
    __NUCLEUS_MACRO_DISPATCH(then, __VA_ARGS__)(__VA_ARGS__)
#define then1(a0) \
    TEST_INSTRUCTION_THEN(a0);
#define then2(a0, a1) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1);
#define then3(a0, a1, a2) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2);
#define then4(a0, a1, a2, a3) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3);
#define then5(a0, a1, a2, a3, a4) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4);
#define then6(a0, a1, a2, a3, a4, a5) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4); \
    TEST_INSTRUCTION_THEN(a5);
#define then7(a0, a1, a2, a3, a4, a5, a6) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4); \
    TEST_INSTRUCTION_THEN(a5); \
    TEST_INSTRUCTION_THEN(a6);
#define then8(a0, a1, a2, a3, a4, a5, a6, a7) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4); \
    TEST_INSTRUCTION_THEN(a5); \
    TEST_INSTRUCTION_THEN(a6); \
    TEST_INSTRUCTION_THEN(a7);
#define then9(a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4); \
    TEST_INSTRUCTION_THEN(a5); \
    TEST_INSTRUCTION_THEN(a6); \
    TEST_INSTRUCTION_THEN(a7); \
    TEST_INSTRUCTION_THEN(a8);
#define then10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    TEST_INSTRUCTION_THEN(a0); \
    TEST_INSTRUCTION_THEN(a1); \
    TEST_INSTRUCTION_THEN(a2); \
    TEST_INSTRUCTION_THEN(a3); \
    TEST_INSTRUCTION_THEN(a4); \
    TEST_INSTRUCTION_THEN(a5); \
    TEST_INSTRUCTION_THEN(a6); \
    TEST_INSTRUCTION_THEN(a7); \
    TEST_INSTRUCTION_THEN(a8); \
    TEST_INSTRUCTION_THEN(a9);
