/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "filesystem.h"
#include "nucleus/common.h"

#include <string>

class VirtualFileSystem : public FileSystem
{
    std::string getHostPath(const std::string& virtual_path);

public:
    VirtualFileSystem(const std::string& mount_point, const std::string& host_path)
    {
        m_mount_point = mount_point;
        m_host_path = host_path;
    }

    // File I/O
    virtual File* openFile(std::string path, OpenMode mode = Read);
    virtual void closeFile(File* handle);
    virtual u64 readFile(File* handle, void* dst, s64 size);
    virtual u64 writeFile(File* handle, const void* src, s64 size);
    virtual u64 seekFile(File* handle, u64 position, SeekMode type);
    virtual bool isOpen(File* handle);
    virtual u64 getFileSize(File* handle);

    // File management
    virtual bool createFile(std::string path);
    virtual bool existsFile(std::string path);
    virtual u64 getFileSize(std::string path);
};
