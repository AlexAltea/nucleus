/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem.h"

namespace fs {

IDevice* VirtualFileSystem::getDevice(const Path& path)
{
    for (auto* device : devices) {
        if (path.compare(0, device->mountPath.length(), device->mountPath) == 0) {
            return device;
        }
    }
    return nullptr;
}

bool VirtualFileSystem::registerDevice(IDevice* device)
{
    if (!device) {
        devices.push_back(device);
    }
    return true;
}

File* VirtualFileSystem::openFile(const Path& path, OpenMode mode)
{
    auto* device = getDevice(path);
    if (!device) {
        return nullptr;
    }

    Path relativePath = path.substr(device->mountPath.length());
    device->openFile(relativePath, mode);
}

}  // namespace fs
