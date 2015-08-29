/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

/**
 * Architectures
 */
#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(_X86_)
#define NUCLEUS_ARCH_X86_32BITS
#endif
#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define NUCLEUS_ARCH_X86_64BITS
#endif
#if defined(NUCLEUS_ARCH_X86_32BITS) || defined(NUCLEUS_ARCH_X86_64BITS)
#define NUCLEUS_ARCH_X86
#endif
#if defined(__arm__) || defined(_M_ARM)
#define NUCLEUS_ARCH_ARM
#endif

/**
 * Platforms
 */
#if defined(__linux__)
#define NUCLEUS_PLATFORM_LINUX
#endif
#if defined(_WIN32) || defined(_WIN64)
#define NUCLEUS_PLATFORM_WINDOWS
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define NUCLEUS_PLATFORM_OSX
#endif
#if defined(__ANDROID_API__)
#define NUCLEUS_PLATFORM_ANDROID
#endif
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define NUCLEUS_PLATFORM_IOS
#endif

/**
 * Compilers
 */
#if defined(_MSC_VER)
#define NUCLEUS_COMPILER_MSVC
#endif
#if defined(__GNUC__)
#define NUCLEUS_COMPILER_GCC
#endif
#if defined(__clang__)
#define NUCLEUS_COMPILER_CLANG
#endif

#include "nucleus/endianness.h"
#include "nucleus/types.h"

/**
 * Events & Status
 */
enum EmulatorEvent
{
    // No event
    NUCLEUS_EVENT_NONE = 0,

    // Emulator events
    NUCLEUS_EVENT_RUN,    // Start Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_PAUSE,  // Pause Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_STOP,   // Stop Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_CLOSE,  // Exit Nucleus
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
 * Endianness
 */
template <typename T>
using LE = NativeEndian<T>;
template <typename T>
using BE = SwappedEndian<T>;

/**
 * Compiler warnings/errors
 */
#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define NOMINMAX  // Conflicts with std::min, std::max in <algorithm> and min, max definitions in <Windows.h>
#endif

#if defined(NUCLEUS_COMPILER_MSVC)
// Reason:       Nucleus is not responsible for useful C11 features not being in C++11.
// Description:  Warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
#endif
