/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem_application.h"
#include "nucleus/filesystem/device/list.h"
#include "nucleus/assert.h"

#include <locale>

namespace fs {

Path ApplicationFileSystem::getPath(Location location) {
    Path path;

#if defined(NUCLEUS_PLATFORM_UWP)
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
#elif defined(NUCLEUS_PLATFORM_WINDOWS)
    assert_always("Unimplemented");
    path = "%temp%";
#endif
    return path;
}

std::unique_ptr<File> ApplicationFileSystem::openFile(Location location, const Path& path, OpenMode mode) {
    Path locationPath = getPath(location);
    auto hostDevice = HostPathDevice("", locationPath);
    return std::unique_ptr<File>(hostDevice.openFile(path, mode));
}

bool ApplicationFileSystem::existsFile(Location location, const Path& path) {
    Path locationPath = getPath(location);
    auto hostDevice = HostPathDevice("", locationPath);
    return hostDevice.existsFile(path);
}

}  // namespace fs
