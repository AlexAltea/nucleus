/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "virtual_filesystem.h"
#include "nucleus/emulator.h"

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
    fopen_s(&file->handler, hostPath.c_str(), getOpenMode(mode));
#else
    file->handler = fopen(hostPath.c_str(), getOpenMode(mode));
#endif

    return file;
}

void VirtualFileSystem::closeFile(File* handle)
{
    std::fclose(handle->handler);
    handle->handler = nullptr;
    delete handle;
}

u64 VirtualFileSystem::readFile(File* handle, void* dst, s64 size)
{
    return std::fread(dst, 1, size, handle->handler);
}

u64 VirtualFileSystem::writeFile(File* handle, const void* src, s64 size)
{
    return std::fwrite(src, 1, size, handle->handler);
}

u64 VirtualFileSystem::seekFile(File* handle, u64 position, SeekMode type)
{
#ifdef NUCLEUS_WIN
    return _fseeki64(handle->handler, position, getSeekMode(type));
#else
    return fseeko64(handle->handler, position, getSeekMode(type));
#endif
}

bool VirtualFileSystem::isOpen(File* handle)
{
    return handle->handler != nullptr;
}

u64 VirtualFileSystem::getFileSize(File* handle)
{
#ifdef NUCLEUS_WIN
    u64 pos = _ftelli64(handle->handler);
    if (_fseeki64(handle->handler, 0, SEEK_END) != 0) {
#else
    u64 pos = ftello64(handle->handler);
    if (fseeko64(handle->handler, 0, SEEK_END) != 0) {
#endif
        nucleus.log.error(LOG_FS, "VirtualFileSystem::getFileSize: Seek failed.");
        return 0;
    }

#ifdef NUCLEUS_WIN
    u64 size = _ftelli64(handle->handler);
    if (_fseeki64(handle->handler, pos, SEEK_SET) != 0) {
#else
    u64 size = ftello64(handle->handler);
    if (fseeko64(handle->handler, pos, SEEK_SET) != 0) {
#endif
        nucleus.log.error(LOG_FS, "VirtualFileSystem::getFileSize: Seek failed.");
        return 0;
    }
    return size;
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

u64 VirtualFileSystem::getFileSize(std::string path)
{
    const std::string& hostPath = getHostPath(path);
    std::FILE* file;

#ifdef NUCLEUS_WIN
    fopen_s(&file, hostPath.c_str(), "rb");
    _fseeki64(file, 0, SEEK_END);
    u64 size = _ftelli64(file);
#else
    file = fopen(hostPath.c_str(), "rb");
    fseeko64(file, 0, SEEK_END);
    u64 size = ftello64(file);
#endif

    fclose(file);
    return size;
}
