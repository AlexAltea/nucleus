/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "iso_container_device.h"
#include "nucleus/filesystem/device/iso_container/iso_container_file.h"
#include "nucleus/logger/logger.h"

namespace fs {

ISOContainerDevice::ISOContainerDevice(const Path& mountPath, File* isoFile)
    : IDevice(mountPath), isoFile(isoFile)
{
}

File* ISOContainerDevice::openFile(const Path& path, OpenMode mode)
{
    if (mode != Read) {
        logger.error(LOG_FS, "ISOContainerDevice cannot open files with write permissions");
        return nullptr;
    }

    logger.error(LOG_FS, "ISOContainerDevice::openFile is not implemented");
    return nullptr;
}

bool ISOContainerDevice::existsFile(const Path& path)
{
    logger.error(LOG_FS, "ISOContainerDevice::existsFile is not implemented");
    return false;
}

bool ISOContainerDevice::removeFile(const Path& path)
{
    logger.error(LOG_FS, "ISOContainerDevice cannot remove files");
    return false;
}

}  // namespace fs
