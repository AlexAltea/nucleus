/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/device.h"
#include "nucleus/filesystem/file.h"

#include <mutex>

namespace fs {

class IMGContainerDevice : public Device {
    File* isoFile;

public:
    // Manage concurrent accesses to IMG contents
    std::mutex mutex;

    IMGContainerDevice(const Path& mountPath, File* isoFile);

    File* openFile(const Path& path, OpenMode mode) override;
    bool existsFile(const Path& path);
    bool removeFile(const Path& path);
};

}  // namespace fs
