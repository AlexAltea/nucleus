/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem.h"

namespace fs {

std::unique_ptr<File> HostFileSystem::openFile(const Path& path, OpenMode mode)
{
    auto hostDevice = HostPathDevice("", "");
    return std::unique_ptr<File>(hostDevice.openFile(path, mode));
}

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
    devices.push_back(device);
    return true;
}

File* VirtualFileSystem::openFile(const Path& path, OpenMode mode)
{
    auto* device = getDevice(path);
    if (!device) {
        return nullptr;
    }

    Path relativePath = path.substr(device->mountPath.length());
    return device->openFile(relativePath, mode);
}

bool VirtualFileSystem::createFile(const Path& path)
{
    auto* device = getDevice(path);
    if (!device) {
        return false;
    }

    Path relativePath = path.substr(device->mountPath.length());
    File* file = device->openFile(relativePath, Write);
    if (!file) {
        return false;
    }
    delete file;
    return true;
}

bool VirtualFileSystem::existsFile(const Path& path)
{
    auto* device = getDevice(path);
    if (!device) {
        return nullptr;
    }

    Path relativePath = path.substr(device->mountPath.length());
    return device->existsFile(relativePath);
}

bool VirtualFileSystem::removeFile(const Path& path)
{
    auto* device = getDevice(path);
    if (!device) {
        return nullptr;
    }

    Path relativePath = path.substr(device->mountPath.length());
    return device->removeFile(relativePath);
}

File::Attributes VirtualFileSystem::getFileAttributes(const Path& path)
{
    auto* device = getDevice(path);
    if (!device) {
        return File::Attributes{};
    }

    Path relativePath = path.substr(device->mountPath.length());
    return device->getFileAttributes(relativePath);
}

}  // namespace fs
