/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

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

namespace core {

enum Platform {
    PLATFORM_LINUX,
    PLATFORM_WINDOWS,
    PLATFORM_UWP,
    PLATFORM_OSX,
    PLATFORM_ANDROID,
    PLATFORM_IOS,
};

}  // namespace core
