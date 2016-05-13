/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host_path_device.h"
#include "host_path_file.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#endif

namespace fs {

HostPathDevice::HostPathDevice(const Path& mountPath, const Path& localPath)
    : Device(mountPath), localPath(localPath) {
}

File* HostPathDevice::openFile(const Path& path, OpenMode mode) {
    auto* file = new HostPathFile(localPath + path, mode);
    if (!file || !file->isOpen()) {
        delete file;
        return nullptr;
    }

    return file;
}

bool HostPathDevice::existsFile(const Path& path) {
    std::FILE* handle;
    std::string realPath = localPath + path;

#ifdef NUCLEUS_COMPILER_MSVC
    fopen_s(&handle, realPath.c_str(), "r");
#else
    handle = fopen(realPath.c_str(), "r");
#endif
    if (!handle) {
        return false;
    }

    fclose(handle);
    return true;
}

bool HostPathDevice::removeFile(const Path& path) {
    std::string realPath = localPath + path;
    int result = remove(realPath.c_str());
    if (!remove) {
        return false;
    }

    return true;
}

File::Attributes HostPathDevice::getFileAttributes(const Path& path) {
    return File::Attributes{};
}


std::vector<DirectoryEntry> HostPathDevice::listDirectory(const Path& path) {
    std::vector<DirectoryEntry> result;
#ifdef NUCLEUS_TARGET_WINDOWS    
    WIN32_FIND_DATA ffd;
    HANDLE handle = FindFirstFile((path + "\\*").c_str(), &ffd);
    if (handle == INVALID_HANDLE_VALUE) {
        return result;
    }
    do {
        if (std::strcmp(ffd.cFileName, ".") == 0 ||
            std::strcmp(ffd.cFileName, "..") == 0) {
            continue;
        }
        DirectoryEntry info;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            info.type = ENTRY_TYPE_FOLDER;
        } else {
            info.type = ENTRY_TYPE_FILE;
        }
        info.name = ffd.cFileName;
        result.push_back(info);
    } while (FindNextFile(handle, &ffd) != 0);
    FindClose(handle);
#endif
    return result;
}

}  // namespace fs
