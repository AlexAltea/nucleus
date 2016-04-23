/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/directory.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/filesystem/path.h"

#include <vector>

namespace fs {

class Device {
public:
    Path mountPath;

    U64 blockSize;

    Device(const Path& mountPath) : mountPath(mountPath) {
    }

    virtual File* openFile(const Path& path, OpenMode mode) = 0;
    virtual bool existsFile(const Path& path) = 0;
    virtual bool removeFile(const Path& path) = 0;

    virtual File::Attributes getFileAttributes(const Path& path) = 0;
};

}  // namespace fs
