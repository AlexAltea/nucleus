/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "virtual_filesystem.h"

#include <algorithm>

std::string VirtualFileSystem::getHostPath(const std::string& virtualPath)
{
    std::string hostPath = m_host_path;
    hostPath += virtualPath.substr(m_mount_point.size());

#if defined(NUCLEUS_WIN)
    std::replace(hostPath.begin(), hostPath.end(), '/', '\\');
#endif
    
    return hostPath;
}

File* VirtualFileSystem::openFile(std::string path, OpenMode mode)
{
    File* file = new File();
    file->virtual_path = path;

    const std::string& hostPath = getHostPath(path);

#if defined(NUCLEUS_WIN)
    fopen_s(&file->handler, hostPath.c_str(), gtOpenMode(mode));
#else
    file->handler = fopen(hostPath.c_str(), getOpenMode(mode));
#endif

    return file;
}

void VirtualFileSystem::closeFile(File* handle)
{
    std::fclose(handle->handler);
}

u64 VirtualFileSystem::readFile(File* handle, void* dst, s64 size)
{
    return std::fread(dst, 1, size, handle->handler);
}

u64 VirtualFileSystem::writeFile(File* handle, const void* src, s64 size)
{
    return std::fwrite(src, 1, size, handle->handler);
}

u64 VirtualFileSystem::seekFile(File* handle, s32 position, SeekMode type)
{
    return 0;
}

bool VirtualFileSystem::createFile(std::string path)
{
    const std::string& hostPath = getHostPath(path);
    std::FILE* file;

#if defined(NUCLEUS_WIN)
    fopen_s(&file, hostPath.c_str(), "w");
#else
    file = fopen(hostPath.c_str(), "w");
#endif
    
    if (!file) {
        return false;
    }

    fclose(file);
    return true;
}

bool VirtualFileSystem::existsFile(std::string path)
{
    const std::string& hostPath = getHostPath(path);
    std::FILE* file;

#if defined(NUCLEUS_WIN)
    fopen_s(&file, hostPath.c_str(), "r");
#else
    file = fopen(hostPath.c_str(), "r");
#endif
    
    if (!file) {
        return false;
    }

    fclose(file);
    return true;
}
