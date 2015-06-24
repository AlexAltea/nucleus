/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem.h"
#include "nucleus/emulator.h"

#include <algorithm>
#include <cstring>

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_LINUX)
#include <unistd.h>
#endif

const char* getOpenMode(OpenMode mode)
{
    switch (mode) {
    case Read:       return "rb";
    case Write:      return "wb";
    case ReadWrite:  return "r+b";
    }

    return "r";
}

const int getSeekMode(SeekMode mode)
{
    switch (mode) {
    case SeekSet:  return SEEK_SET;
    case SeekCur:  return SEEK_CUR;
    case SeekEnd:  return SEEK_END;
    }

    return SEEK_SET;
}

FileSystem* getFilesystem(const s8* path)
{
    for (auto& device : nucleus.devices) {
        if (!strncmp(path, device->m_mount_point.data(), device->m_mount_point.size())) {
            return device;
        }
    }
    return nullptr;
}


/**
 * Get the path to the folder that contains the Nucleus executable
 */
std::string getEmulatorPath()
{
    char buffer[4096];
#ifdef NUCLEUS_PLATFORM_WINDOWS
    GetModuleFileName(NULL, buffer, sizeof(buffer));
#endif

    std::string exePath = buffer;
#ifdef NUCLEUS_PLATFORM_WINDOWS
    int pos = exePath.rfind('\\');
#else
    int pos = exePath.rfind('/');
#endif

    return exePath.substr(0, pos+1);
}

/**
 * Get the path to the folder that contains the emulated ELF binary
 */
std::string getProcessPath(const std::string& elfPath)
{
    // Get current working directory
    char buffer[4096];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    GetCurrentDirectory(sizeof(buffer), buffer);
#elif defined(NUCLEUS_PLATFORM_LINUX)
    getcwd(buffer, sizeof(buffer));
#endif

    // Check if elfPath is absolute
    std::string procPath;
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
    int pos = procPath.rfind('\\');
#else
    int pos = procPath.rfind('/');
#endif

    return procPath.substr(0, pos+1);
}
