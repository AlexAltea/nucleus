/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vfsLocalFile.h"
#include <iostream>

static const char* getOpenMode(vfsOpenMode mode)
{
    switch (mode) {
    case vfsRead:        return "rb";
    case vfsWrite:       return "wb";
    case vfsReadWrite:   return "r+b";
    }

    return "r";
}

static const int getSeekMode(vfsSeekMode mode)
{
    switch (mode) {
    case vfsSeekSet:  return SEEK_SET;
    case vfsSeekCur:  return SEEK_CUR;
    case vfsSeekEnd:  return SEEK_END;
    }

    return SEEK_SET;
}

vfsLocalFile::vfsLocalFile()
{
    m_file = nullptr;
}

vfsLocalFile::vfsLocalFile(const std::string& path, vfsOpenMode mode)
{
    Open(path, mode);
}

vfsLocalFile::~vfsLocalFile()
{
    Close();
}

bool vfsLocalFile::Open(const std::string& path, vfsOpenMode mode)
{
    Close();

#if defined(_WIN32) && defined(UNICODE)
    _wfopen_s(&m_file, ConvertUTF8ToWString(path).c_str(), ConvertUTF8ToWString(getOpenMode(mode)).c_str());
#elif defined(_WIN32)
    fopen_s(&m_file, path.c_str(), getOpenMode(mode));
#else
    m_file = fopen(path.c_str(), getOpenMode(mode));
#endif

    return IsOpen();
}

bool vfsLocalFile::Close()
{
    if (!IsOpen() || std::fclose(m_file) != 0) {
        m_file = nullptr;
        return false;
    }

    m_file = nullptr;
    return true;
}

u64 vfsLocalFile::GetSize()
{
#ifdef NUCLEUS_WIN
    u64 pos = _ftelli64(m_file);
    if (_fseeki64(m_file, 0, SEEK_END) != 0) {
#else
    u64 pos = ftello64(m_file);
    if (fseeko64(m_file, 0, SEEK_END) != 0) {
#endif
        std::cerr << "vfsLocalFile::GetSize: Seek failed." << std::endl;
        return 0;
    }

#ifdef NUCLEUS_WIN
    u64 size = _ftelli64(m_file);
    if (_fseeki64(m_file, 0, SEEK_END) != 0) {
#else
    u64 size = ftello64(m_file);
    if (fseeko64(m_file, 0, SEEK_END) != 0) {
#endif
        std::cerr << "vfsLocalFile::GetSize: Seek failed." << std::endl;
        return 0;
    }
    return size;
}

bool vfsLocalFile::Write(const void* src, u64 size)
{
    if (!IsOpen() || std::fwrite(src, 1, size, m_file) != size) {
        return false;
    }
    return true;
}

bool vfsLocalFile::Read(void* dst, u64 size)
{
    if (!IsOpen() || std::fread(dst, 1, size, m_file) != size) {
        return false;
    }
    return true;
}

bool vfsLocalFile::Seek(s64 offset, vfsSeekMode mode)
{
#ifdef NUCLEUS_WIN
    if (!IsOpen() || _fseeki64(m_file, offset, getSeekMode(mode)) != 0) {
#else
    if (!IsOpen() || fseeko64(m_file, offset, getSeekMode(mode)) != 0) {
#endif
        return false;
    }
    return true;
}

u64 vfsLocalFile::Tell() const
{
    if (IsOpen()) {
#ifdef NUCLEUS_WIN
        return _ftelli64(m_file);
#else
        return ftello64(m_file);
#endif
    }
    else {
        return -1;
    }
}

bool vfsLocalFile::IsOpen() const
{
    return m_file != nullptr;
}
