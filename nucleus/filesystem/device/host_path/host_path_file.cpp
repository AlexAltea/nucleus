/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host_path_file.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define fseeko64 _fseeki64
#define ftello64 _ftelli64
#endif

namespace fs {

// Transform Nucleus filesystem open mode into POSIX open mode
const char* getOpenMode(OpenMode mode)
{
    switch (mode) {
    case Read:
        return "rb";
    case Write:
        return "wb";
    case ReadWrite:
        return "r+b";
    }

    return "r";
}

// Transform Nucleus filesystem seek mode into POSIX seek mode
const int getSeekMode(SeekMode mode)
{
    switch (mode) {
    case SeekSet:
        return SEEK_SET;
    case SeekCur:
        return SEEK_CUR;
    case SeekEnd:
        return SEEK_END;
    }

    return SEEK_SET;
}

HostPathFile::HostPathFile(const Path& path, OpenMode mode)
{
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    fopen_s(&handle, path.c_str(), getOpenMode(mode));
#else
    handle = fopen(path.c_str(), getOpenMode(mode));
#endif
}

HostPathFile::~HostPathFile()
{
    std::fclose(handle);
}

Size HostPathFile::read(void* dst, Size size)
{
    return std::fread(dst, 1, size, handle);
}

Size HostPathFile::write(const void* src, Size size)
{
    return std::fwrite(src, 1, size, handle);
}

Position HostPathFile::seek(Position pos, SeekMode mode)
{
    return fseeko64(handle, pos, getSeekMode(mode));
}

Position HostPathFile::tell()
{
    return ftello64(handle);
}

bool HostPathFile::isOpen()
{
    return handle != nullptr;
}

}  // namespace fs
