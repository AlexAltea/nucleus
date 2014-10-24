/**
 * (c) 2014 Nucleus project. All rights reserved.
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
    case 0x464c457f:
        return FILETYPE_ELF;
    case 0x00454353:
        return FILETYPE_SELF;
    case 0x21726152:
        return FILETYPE_RAR;
    case 0x04034b50:
    case 0x06054b50:
    case 0x08074b50:
        return FILETYPE_ZIP;
    }
    return FILETYPE_UNKNOWN;
}
