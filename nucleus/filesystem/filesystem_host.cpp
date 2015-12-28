/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem_host.h"
#include "nucleus/filesystem/device/list.h"

namespace fs {

std::unique_ptr<File> HostFileSystem::openFile(const Path& path, OpenMode mode) {
    auto hostDevice = HostPathDevice("", "");
    return std::unique_ptr<File>(hostDevice.openFile(path, mode));
}

bool HostFileSystem::existsFile(const Path& path) {
    auto hostDevice = HostPathDevice("", "");
    return hostDevice.existsFile(path);
}

}  // namespace fs
