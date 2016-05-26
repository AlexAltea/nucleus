/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Target detection
#if defined(__ANDROID__)
#define NUCLEUS_TARGET_ANDROID
#endif
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define NUCLEUS_TARGET_IOS
#endif
#if defined(__linux__) && !defined(__ANDROID__)
#define NUCLEUS_TARGET_LINUX
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define NUCLEUS_TARGET_OSX
#endif
#if defined(_NUCLEUS_TARGET_UWP)
#define NUCLEUS_TARGET_UWP
#endif
#if (defined(_WIN32) || defined(_WIN64)) && !defined(_NUCLEUS_TARGET_UWP) && !defined(__ANDROID__)
#define NUCLEUS_TARGET_WINDOWS
#endif

namespace core {

enum Target {
    TARGET_ANDROID,
    TARGET_IOS,
    TARGET_LINUX,
    TARGET_OSX,
    TARGET_UWP,
    TARGET_WINDOWS,
};

}  // namespace core
