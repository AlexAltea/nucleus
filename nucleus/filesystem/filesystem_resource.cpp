/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem_resource.h"
#include "nucleus/filesystem/device/list.h"

namespace fs {

std::unique_ptr<File> ResourceFileSystem::openFile(const Path& path, OpenMode mode) {
    auto hostDevice = HostPathDevice("", "");
    return std::unique_ptr<File>(hostDevice.openFile(path, mode));
}

bool ResourceFileSystem::existsFile(const Path& path) {
    auto hostDevice = HostPathDevice("", "");
    return hostDevice.existsFile(path);
}

}  // namespace fs
