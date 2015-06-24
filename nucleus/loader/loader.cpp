/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"
#include "nucleus/filesystem/filesystem.h"

Filetype detectFiletype(const std::string& filepath)
{
    FileSystem* fs = getFilesystem(filepath.c_str());
    File* file = fs->openFile(filepath, Read);
    be_t<u32> magic;

    if (!fs->isOpen(file) || !fs->readFile(file, &magic, sizeof(magic))) {
        return FILETYPE_ERROR;
    }
    fs->closeFile(file);

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
