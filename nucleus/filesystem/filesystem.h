/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

enum OpenMode
{
    Read = 0x1,
    Write = 0x2,
    Excl = 0x4,
    Append = 0x8,
    ReadWrite = Read | Write,
    WriteExcl = Write | Excl,
    WriteAppend = Write | Append,
};

enum SeekMode
{
    SeekSet,
    SeekCur,
    SeekEnd,
};

struct File
{
    std::FILE* handler;
    std::string virtual_path;
};

class FileSystem
{
public:
    std::string m_mount_point;
    std::string m_host_path;

    virtual ~FileSystem() {}

    // File I/O
    virtual File* openFile(const std::string& path, OpenMode mode = Read) = 0;
    virtual void closeFile(File* handle) = 0;
    virtual u64 readFile(File* handle, void* dst, s64 size) = 0;
    virtual u64 writeFile(File* handle, const void* src, s64 size) = 0;
    virtual u64 seekFile(File* handle, u64 position, SeekMode type) = 0;
    virtual bool isOpen(File* handle) = 0;
    virtual u64 getFileSize(File* handle) = 0;

    // File management
    virtual bool createFile(const std::string& path) = 0;
    virtual bool existsFile(const std::string& path) = 0;
    virtual u64 getFileSize(const std::string& path) = 0;
};

const char* getOpenMode(OpenMode mode);
const int getSeekMode(SeekMode mode);
FileSystem* getFilesystem(const s8* path);

// Utilities
std::string getEmulatorPath();
std::string getProcessPath(const std::string& elfPath);
