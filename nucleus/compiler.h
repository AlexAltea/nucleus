/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#if defined(_MSC_VER)
#define NUCLEUS_COMPILER_MSVC
#endif
#if defined(__GNUC__)
#define NUCLEUS_COMPILER_GCC
#endif
#if defined(__clang__)
#define NUCLEUS_COMPILER_CLANG
#endif
