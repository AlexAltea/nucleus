/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Architectures
#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define NUCLEUS_X86_64
#endif

// Platforms
#if defined(__linux__)
#define NUCLEUS_LINUX
#endif
#if defined(_WIN32) || defined(_WIN64)
#define NUCLEUS_WIN
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define NUCLEUS_MACOS
#endif

// Types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
