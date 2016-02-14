/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

enum Format {
#define FORMAT(name, ...) FORMAT_##name,
#include "format.inl"
#undef FORMAT

    _FORMAT_COUNT,
};

struct FormatInfo {
    Format format;
    Size bytesPerPixel;
};

extern const FormatInfo formatInfo[_FORMAT_COUNT];

}  // namespace gfx
