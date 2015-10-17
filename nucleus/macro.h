/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Count arguments
#define __NUCLEUS_ARGUMENT_EXPAND(x) x
#define __NUCLEUS_ARGUMENT_COUNT(...) \
    __NUCLEUS_ARGUMENT_EXPAND(__NUCLEUS_ARGUMENT_EXTRACT(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define __NUCLEUS_ARGUMENT_EXTRACT(a1, a2, a3, a4, a5, a6, a7, a8, N, ...) N

// Dispatching macros
#define __NUCLEUS_MACRO_DISPATCH(function, ...) \
    __NUCLEUS_MACRO_SELECT(function, __NUCLEUS_ARGUMENT_COUNT(__VA_ARGS__))
#define __NUCLEUS_MACRO_SELECT(function, argc) \
    __NUCLEUS_MACRO_CONCAT(function, argc)
#define __NUCLEUS_MACRO_CONCAT(a, b) a##b
