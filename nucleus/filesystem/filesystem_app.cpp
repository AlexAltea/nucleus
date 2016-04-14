/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem_app.h"
#include "nucleus/filesystem/device/list.h"
#include "nucleus/assert.h"

#include <locale>

namespace fs {

Path AppFileSystem::getPath(Location location) {
    Path path;

#if defined(NUCLEUS_TARGET_LINUX)
    switch (location) {
    case APPLICATION_LOCATION_LOCAL:
        path = "./"; break;
    case APPLICATION_LOCATION_ROAMING:
        path = "./"; break;
    case APPLICATION_LOCATION_TEMP:
        path = "/tmp/"; break;
    default:
        assert_always("Unexpected");
    }

#elif defined(NUCLEUS_TARGET_UWP)
    Windows::Storage::StorageFolder^ folder;
    switch (location) {
    case APPLICATION_LOCATION_LOCAL:
        folder = Windows::Storage::ApplicationData::Current->LocalFolder; break;
    case APPLICATION_LOCATION_ROAMING:
        folder = Windows::Storage::ApplicationData::Current->RoamingFolder; break;
    case APPLICATION_LOCATION_TEMP:
        folder = Windows::Storage::ApplicationData::Current->TemporaryFolder; break;
    }
    std::wstring widePath = folder->Path->Data();
    std::string narrowPath(widePath.begin(), widePath.end()); // HACK: Implement proper Path class
    path = narrowPath;

#elif defined(NUCLEUS_TARGET_WINDOWS)
    switch (location) {
    case APP_LOCATION_LOCAL:
        path = "./"; break;
    case APP_LOCATION_ROAMING:
        path = "./"; break;
    case APP_LOCATION_TEMP:
        path = "%TEMP%/"; break;
    default:
        assert_always("Unexpected");
    }
#endif
    return path;
}

std::unique_ptr<File> AppFileSystem::openFile(Location location, const Path& path, OpenMode mode) {
    Path locationPath = getPath(location);
    auto hostDevice = HostPathDevice("", locationPath);
    return std::unique_ptr<File>(hostDevice.openFile(path, mode));
}

bool AppFileSystem::existsFile(Location location, const Path& path) {
    Path locationPath = getPath(location);
    auto hostDevice = HostPathDevice("", locationPath);
    return hostDevice.existsFile(path);
}

}  // namespace fs
