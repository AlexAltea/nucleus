/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include <string>

enum vfsOpenMode
{
	vfsRead = 0x1,
	vfsWrite = 0x2,
	vfsExcl = 0x4,
	vfsAppend = 0x8,
	vfsReadWrite = vfsRead | vfsWrite,
	vfsWriteExcl = vfsWrite | vfsExcl,
	vfsWriteAppend = vfsWrite | vfsAppend,
};

enum vfsSeekMode
{
    vfsSeekSet,
    vfsSeekCur,
    vfsSeekEnd,
};

class vfsLocalFile
{
	std::FILE* m_file;

public:
    vfsLocalFile();
	vfsLocalFile(const std::string& path, vfsOpenMode mode = vfsRead);
    ~vfsLocalFile();

	bool Open(const std::string& path, vfsOpenMode mode = vfsRead);
    bool IsOpen() const;
	bool Close();

    bool Read(void* dst, u64 size);
	bool Write(const void* src, u64 size);
	bool vfsLocalFile::Seek(s64 offset, vfsSeekMode mode);
	u64 Tell() const;
    u64 GetSize();
};
