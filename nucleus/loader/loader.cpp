/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"
#include "nucleus/filesystem/vfsLocalFile.h"

Filetype detectFiletype(const std::string& filepath)
{
    vfsLocalFile file;
    be_t<u32> magic;

    if (!file.Open(filepath, vfsRead) ||
        !file.Read(&magic, sizeof(magic))) {
        return FILETYPE_ERROR;
    }

    switch (magic.ToBE()) {
    case '\x7f\x45LF':
        return FILETYPE_ELF;
    case 'SCE\x00':
        return FILETYPE_SELF;
    case '!raR':
        return FILETYPE_RAR;
    case '\x04\x03KP':
    case '\x06\x05KP':
    case '\x08\x07KP':
        return FILETYPE_ZIP;
    }
    return FILETYPE_UNKNOWN;
}
