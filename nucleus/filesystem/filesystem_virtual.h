/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/device/list.h"
#include "nucleus/filesystem/device.h"
#include "nucleus/filesystem/directory.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/filesystem/path.h"

#include <memory>
#include <vector>

namespace fs {

/**
 * Virtual File System
 * ===================
 * Aggregates multiple devices which are mapped to different path prefixes.
 */
class VirtualFileSystem {
    std::vector<Device*> devices;

    // Find an appropriate device to handle the given path
    Device* getDevice(const Path& path);

public:
    bool registerDevice(Device* device);

    File* openFile(const Path& path, OpenMode mode);
    bool createFile(const Path& path);
    bool existsFile(const Path& path);
    bool removeFile(const Path& path);

    Directory* openDir(const Path& path);
    bool existsDir(const Path& path);
    bool removeDir(const Path& path);

    File::Attributes getFileAttributes(const Path& path);
};

}  // namespace fs
