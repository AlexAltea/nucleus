/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "utils.h"

#include <algorithm>

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_LINUX)
#include <unistd.h>
#endif

namespace fs {

Path getEmulatorPath()
{
    char buffer[4096];
#ifdef NUCLEUS_PLATFORM_WINDOWS
    GetModuleFileName(NULL, buffer, sizeof(buffer));
#endif

    Path exePath = buffer;
#ifdef NUCLEUS_PLATFORM_WINDOWS
    size_t pos = exePath.rfind('\\');
#else
    size_t pos = exePath.rfind('/');
#endif

    return exePath.substr(0, pos+1);
}

Path getProcessPath(const Path& elfPath)
{
    // Get current working directory
    char buffer[4096];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    GetCurrentDirectory(sizeof(buffer), buffer);
#elif defined(NUCLEUS_PLATFORM_LINUX)
    getcwd(buffer, sizeof(buffer));
#endif

    // Check if elfPath is absolute
    Path procPath;
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    if (elfPath.find(':') != std::string::npos) {
#else
    if (elfPath[0] == '/') {
#endif
        procPath = elfPath;
    } else {
        procPath = buffer + ('/' + elfPath);
    }

    // Get the path to the actual folder containing the ELF binary
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    std::replace(procPath.begin(), procPath.end(), '/', '\\');
    size_t pos = procPath.rfind('\\');
#else
    size_t pos = procPath.rfind('/');
#endif

    return procPath.substr(0, pos+1);
}

}  // namespace fs
