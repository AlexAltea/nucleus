/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#include <VersionHelpers.h>
#endif

namespace core {

Host::Host() {
    initialize();
}

void Host::initialize() {
    initCPU();
    initGPU();
    initOS();
}

void Host::initCPU() {
}

void Host::initGPU() {
}

void Host::initOS() {
#ifdef NUCLEUS_TARGET_ANDROID
    os.name = "Android";
#endif
#ifdef NUCLEUS_TARGET_IOS
    os.name = "iOS";
#endif
#ifdef NUCLEUS_TARGET_LINUX
    os.name = "Linux";
#endif
#ifdef NUCLEUS_TARGET_OSX
    os.name = "OSX";
#endif
#ifdef NUCLEUS_TARGET_UWP
    os.name = "Windows";
#endif
#ifdef NUCLEUS_TARGET_WINDOWS
    if (IsWindows10OrGreater()) {
        os.name = "Windows 10";
    } else if (IsWindows8Point1OrGreater()) {
        os.name = "Windows 8.1";
    } else if (IsWindows8OrGreater()) {
        os.name = "Windows 8";
    } else if (IsWindows7OrGreater()) {
        os.name = "Windows 7";
    } else if (IsWindowsVistaSP2OrGreater()) {
        os.name = "Windows Vista SP2";
    } else if (IsWindowsVistaSP1OrGreater()) {
        os.name = "Windows Vista SP1";
    } else if (IsWindowsVistaOrGreater()) {
        os.name = "Windows Vista";
    } else if (IsWindowsXPSP3OrGreater()) {
        os.name = "Windows XP SP3";
    } else if (IsWindowsXPSP2OrGreater()) {
        os.name = "Windows XP SP2";
    } else if (IsWindowsXPSP1OrGreater()) {
        os.name = "Windows XP SP1";
    } else if (IsWindowsXPOrGreater()) {
        os.name = "Windows XP";
    }
#endif
}

}  // namespace core
