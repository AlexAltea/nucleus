/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/filesystem/path.h"

#include <cstdio>

namespace fs {

class HostPathFile : public File {
    std::FILE* handle = nullptr;

public:
    HostPathFile(const Path& path, OpenMode mode);
    ~HostPathFile();

    virtual Size read(void* dst, Size size) override;
    virtual Size write(const void* src, Size size) override;
    virtual void seek(Position pos, SeekMode mode) override;
    virtual Position tell() override;
    virtual Attributes attributes() override;

    bool isOpen();
};

}  // namespace fs
