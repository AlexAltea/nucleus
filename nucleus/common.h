/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

/**
 * Architectures
 */
#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define NUCLEUS_X86_64
#endif

/**
 * Platforms
 */
#if defined(__linux__)
#define NUCLEUS_LINUX
#endif
#if defined(_WIN32) || defined(_WIN64)
#define NUCLEUS_WIN
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define NUCLEUS_MACOS
#endif

/**
 * Events & Status
 */
enum EmulatorEvent
{
    // No event
    NUCLEUS_EVENT_NONE = 0,

    // Emulator events
    NUCLEUS_EVENT_RUN,
    NUCLEUS_EVENT_PAUSE,
    NUCLEUS_EVENT_STOP,
    NUCLEUS_EVENT_CLOSE,
};

enum EmulatorStatus
{
    // No status
    NUCLEUS_STATUS_UNKNOWN = 0,

    // Emulator status
    NUCLEUS_STATUS_RUNNING,
    NUCLEUS_STATUS_PAUSED,
    NUCLEUS_STATUS_STOPPED,
    NUCLEUS_STATUS_CLOSED,
};

/**
 * Types
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;

struct u128
{
    u64 lo, hi;

    bool operator == (const u128& r) const { return (lo == r.lo) && (hi == r.hi); }
    bool operator != (const u128& r) const { return (lo != r.lo) || (hi != r.hi); }

    u128 operator | (const u128& r) const { return u128{lo | r.lo, hi | r.hi}; }
    u128 operator & (const u128& r) const { return u128{lo & r.lo, hi & r.hi}; }
    u128 operator ^ (const u128& r) const { return u128{lo ^ r.lo, hi ^ r.hi}; }
    u128 operator ~ () const { return u128{~lo, ~hi}; }
};

/**
 * Endianness
 */
#include "externals/BEType.h"
