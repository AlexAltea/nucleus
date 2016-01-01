/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host_path_device.h"
#include "host_path_file.h"

namespace fs {

HostPathDevice::HostPathDevice(const Path& mountPath, const Path& localPath)
    : Device(mountPath), localPath(localPath)
{
}

File* HostPathDevice::openFile(const Path& path, OpenMode mode)
{
    auto* file = new HostPathFile(localPath + path, mode);
    if (!file->isOpen()) {
        delete file;
        return nullptr;
    }

    return file;
}

bool HostPathDevice::existsFile(const Path& path)
{
    std::FILE* handle;
    std::string realPath = localPath + path;
    fopen_s(&handle, realPath.c_str(), "r");
    if (!handle) {
        return false;
    }

    fclose(handle);
    return true;
}

bool HostPathDevice::removeFile(const Path& path)
{
    std::string realPath = localPath + path;
    int result = remove(realPath.c_str());
    if (!remove) {
        return false;
    }

    return true;
}

File::Attributes HostPathDevice::getFileAttributes(const Path& path)
{
    return File::Attributes{};
}

}  // namespace fs
