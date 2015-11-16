/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/device.h"
#include "nucleus/filesystem/directory.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/filesystem/path.h"

// Devices
#include "device/host_path/host_path_device.h"
#include "device/iso_container/iso_container_device.h"

#include <memory>
#include <string>
#include <vector>

namespace fs {

class HostFileSystem {
public:
    static std::unique_ptr<File> openFile(const Path& path, OpenMode mode);
    static bool createFile(const Path& path);
    static bool existsFile(const Path& path);
    static bool removeFile(const Path& path);
};

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

    IDir* openDir(const Path& path);
    bool existsDir(const Path& path);
    bool removeDir(const Path& path);

    File::Attributes getFileAttributes(const Path& path);
};

}  // namespace fs
