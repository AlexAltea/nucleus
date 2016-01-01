/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/device.h"

namespace fs {

class HostPathDevice : public Device {
    Path localPath;

public:
    HostPathDevice(const Path& mountPath, const Path& localPath);

    File* openFile(const Path& path, OpenMode mode) override;
    bool existsFile(const Path& path) override;
    bool removeFile(const Path& path) override;

    File::Attributes getFileAttributes(const Path& path) override;
};

}  // namespace fs
