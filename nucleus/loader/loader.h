/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/filesystem/file.h"

#include <string>

enum Filetype {
    // Error
    FILETYPE_ERROR,

    // PS3 file types
    FILETYPE_ELF,
    FILETYPE_SELF,
    FILETYPE_PRX,
    FILETYPE_SPRX,
    FILETYPE_PKG,
    FILETYPE_PSF,
    FILETYPE_TRP,

    // Common file types
    FILETYPE_ISO,
    FILETYPE_RAR,
    FILETYPE_ZIP,

    // Unknown file type
    FILETYPE_UNKNOWN,
};

// Detect the format of a file given its host path
Filetype detectFiletype(const std::string& filename);

// Detect the format of a file given its handler
Filetype detectFiletype(fs::File* file);
