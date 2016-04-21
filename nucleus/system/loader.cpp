/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"
#include "nucleus/assert.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/filesystem_host.h"

#include "nucleus/system/scei/cellos/cellos_loader.h"
#include "nucleus/system/scei/orbisos/orbis_loader.h"

Filetype detectFiletype(const std::string& filepath) {
    auto file = fs::HostFileSystem::openFile(filepath, fs::Read);
    return detectFiletype(file.get());
}

Filetype detectFiletype(fs::File* file) {
    // Magic value to check
    BE<U32> magic;

    file->seek(0, fs::SeekSet);
    if (!file->read(&magic, sizeof(magic))) {
        return FILETYPE_ERROR;
    }

    switch (magic.ToBE()) {
    case 0x464C457F:
        return FILETYPE_ELF;
    case 0x00454353:
        return FILETYPE_SELF;
    case 0x00505346:
        return FILETYPE_PSF;
    case 0x21726152:
        return FILETYPE_RAR;
    case 0x04034B50:
    case 0x06054B50:
    case 0x08074B50:
        return FILETYPE_ZIP;
    }

    return FILETYPE_UNKNOWN;
}

core::Platform detectPlatform(fs::File* file) {
    std::vector<core::Platform> platforms;
#ifdef NUCLEUS_PLATFORM_PS3
    if (sys::scei::cellos::isValid(file)) {
        platforms.push_back(core::PLATFORM_PS3);
    }
#endif
#ifdef NUCLEUS_PLATFORM_PS4
    if (sys::scei::orbis::isValid(file)) {
        platforms.push_back(core::PLATFORM_PS4);
    }
#endif

    if (platforms.size() > 0) {
        assert_true(platforms.size() == 1, "Two or more platforms recognize the file as its own");
        return platforms[0];
    } else {
        return core::PLATFORM_UNKNOWN;
    }
}
