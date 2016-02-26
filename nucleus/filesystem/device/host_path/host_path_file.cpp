/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host_path_file.h"
#include "nucleus/assert.h"

#if defined(NUCLEUS_COMPILER_MSVC)
#define fseeko64 _fseeki64
#define ftello64 _ftelli64
#endif
#if defined(NUCLEUS_PLATFORM_ANDROID)
#define fseeko64 fseek
#define ftello64 ftell
#endif

namespace fs {

// Transform Nucleus filesystem open mode into POSIX open mode
const char* getOpenMode(OpenMode mode) {
    switch (mode) {
    case Read:
        return "rb";
    case Write:
        return "wb";
    case ReadWrite:
        return "r+b";
    default:
        assert_always("Unexpected");
        return "r";
    }
}

// Transform Nucleus filesystem seek mode into POSIX seek mode
const int getSeekMode(SeekMode mode) {
    switch (mode) {
    case SeekSet:
        return SEEK_SET;
    case SeekCur:
        return SEEK_CUR;
    case SeekEnd:
        return SEEK_END;
    default:
        assert_always("Unexpected");
        return SEEK_SET;
    }
}

HostPathFile::HostPathFile(const Path& path, OpenMode mode) {
#if defined(NUCLEUS_COMPILER_MSVC)
    fopen_s(&handle, path.c_str(), getOpenMode(mode));
#else
    handle = fopen(path.c_str(), getOpenMode(mode));
#endif
}

HostPathFile::~HostPathFile() {
    std::fclose(handle);
}

Size HostPathFile::read(void* dst, Size size) {
    return std::fread(dst, 1, size, handle);
}

Size HostPathFile::write(const void* src, Size size) {
    return std::fwrite(src, 1, size, handle);
}

Position HostPathFile::seek(Position pos, SeekMode mode) {
    return fseeko64(handle, pos, getSeekMode(mode));
}

Position HostPathFile::tell() {
    return ftello64(handle);
}

File::Attributes HostPathFile::attributes() {
    // TODO
    File::Attributes attr;
    attr.timestamp_access = 0;
    attr.timestamp_create = 0;
    attr.timestamp_write = 0;

    // Obtain size
    Position originalPosition = tell();
    seek(0, fs::SeekEnd);
    attr.size = tell();
    seek(originalPosition, fs::SeekSet);

    return attr;
}

bool HostPathFile::isOpen() {
    return handle != nullptr;
}

}  // namespace fs
