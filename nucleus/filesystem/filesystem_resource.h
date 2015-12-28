/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/device.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/filesystem/path.h"

#include <memory>

namespace fs {

/**
 * Resource File System
 * ====================
 * Allows access to the resources bundles with this executable.
 */
class ResourceFileSystem {
public:
    static std::unique_ptr<File> openFile(const Path& path, OpenMode mode);
    static bool createFile(const Path& path);
    static bool existsFile(const Path& path);
    static bool removeFile(const Path& path);
};

}  // namespace fs
