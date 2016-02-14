/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/architecture.h"
#include "nucleus/platform.h"
#include "nucleus/compiler.h"
#include "nucleus/endianness.h"
#include "nucleus/feature.h"
#include "nucleus/target.h"
#include "nucleus/types.h"

/**
 * Builds
 */
#if defined(_DEBUG)
#define NUCLEUS_BUILD_DEBUG
#endif
#if defined(NDEBUG)
#define NUCLEUS_BUILD_RELEASE
#endif

/**
 * Events & Status
 */
enum EmulatorEvent {
    // No event
    NUCLEUS_EVENT_NONE = 0,

    // Emulator events
    NUCLEUS_EVENT_RUN,    // Start Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_PAUSE,  // Pause Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_STOP,   // Stop Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_CLOSE,  // Exit Nucleus
};

enum EmulatorStatus {
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
// Reason:       Nucleus is not responsible for useful C11 features not being in the latest C++ revision
// Description:  Warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
#endif
