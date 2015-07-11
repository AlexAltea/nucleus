/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "iso_container_file.h"
#include "nucleus/logger/logger.h"

namespace fs {

ISOContainerFile::ISOContainerFile(ISOContainerDevice* parent)
    : parent(parent)
{
}

ISOContainerFile::~ISOContainerFile()
{
}

Size ISOContainerFile::read(void* dst, Size size)
{
    std::lock_guard<std::mutex> lock(parent->mutex);
}

Size ISOContainerFile::write(const void* src, Size size)
{
    logger.error(LOG_FS, "ISOContainerFile cannot be written to");
    return 0;
}

Position ISOContainerFile::seek(Position pos, SeekMode mode)
{
}

Position ISOContainerFile::tell()
{
}

}  // namespace fs
