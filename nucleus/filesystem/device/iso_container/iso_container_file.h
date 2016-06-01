/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/filesystem/device/iso_container/iso_container_device.h"

namespace fs {

class ISOContainerFile : public File {
    ISOContainerDevice* parent;

public:
    ISOContainerFile(ISOContainerDevice* parent);
    ~ISOContainerFile();

    virtual Size read(void* dst, Size size) override;
    virtual Size write(const void* src, Size size) override;
    virtual void seek(Position pos, SeekMode mode) override;
    virtual Position tell() override;
    virtual Attributes attributes() override;
};

}  // namespace fs
