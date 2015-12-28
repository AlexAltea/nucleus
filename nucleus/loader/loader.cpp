/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/filesystem_host.h"

Filetype detectFiletype(const std::string& filepath)
{
    auto file = fs::HostFileSystem::openFile(filepath, fs::Read);
    return detectFiletype(file.get());
}

Filetype detectFiletype(fs::File* file)
{
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
