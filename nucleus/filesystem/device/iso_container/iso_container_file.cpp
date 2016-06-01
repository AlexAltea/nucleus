/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "iso_container_file.h"
#include "nucleus/logger/logger.h"

namespace fs {

ISOContainerFile::ISOContainerFile(ISOContainerDevice* parent)
    : parent(parent) {
}

ISOContainerFile::~ISOContainerFile() {
}

Size ISOContainerFile::read(void* dst, Size size) {
    std::lock_guard<std::mutex> lock(parent->mutex);

    logger.warning(LOG_FS, "ISOContainerFile::read is not implemented");
    return 0;
}

Size ISOContainerFile::write(const void* src, Size size) {
    logger.error(LOG_FS, "ISOContainerFile cannot be written to");
    return 0;
}

void ISOContainerFile::seek(Position pos, SeekMode mode) {
    logger.warning(LOG_FS, "ISOContainerFile::seek is not implemented");
}

Position ISOContainerFile::tell() {
    logger.warning(LOG_FS, "ISOContainerFile::tell is not implemented");
    return 0;
}

File::Attributes ISOContainerFile::attributes() {
    // TODO
    return {};
}

}  // namespace fs
