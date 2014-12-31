/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

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

Filetype detectFiletype(const std::string& filename);
