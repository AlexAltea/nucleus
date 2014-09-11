/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"
#include "nucleus/filesystem/local_file.h"

Filetype detectFiletype(const std::string& filepath)
{
    vfsLocalFile file;
    be_t<u32> magic;

    if (!file.Open(filepath, vfsRead) ||
        !file.Read(&magic, sizeof(magic))) {
        return FILETYPE_ERROR;
    }

    switch (magic.ToBE()) {
    case 0x7f454c46:
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
