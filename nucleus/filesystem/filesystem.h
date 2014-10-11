/**
 * (c) 2014 Nucleus project. All rights reserved.
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
    virtual File* openFile(std::string path, OpenMode mode = Read) = 0;
    virtual void closeFile(File* handle) = 0;
    virtual u64 readFile(File* handle, void* dst, s64 size) = 0;
    virtual u64 writeFile(File* handle, const void* src, s64 size) = 0;
    virtual u64 seekFile(File* handle, s32 position, SeekMode type) = 0;

    // File management
    virtual bool createFile(std::string path) = 0;
    virtual bool existsFile(std::string path) = 0;
};

const char* gtOpenMode(OpenMode mode);
const int gtSeekMode(SeekMode mode);
