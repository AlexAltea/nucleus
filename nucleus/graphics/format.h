/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

enum Format {
    FORMAT_R32G32,
    FORMAT_R32G32B32,
    FORMAT_R32G32B32A32,
    FORMAT_R8G8B8A8_UNORM,

    _FORMAT_COUNT,
};

struct FormatInfo {
    Format format;
    Size bytesPerPixel;
};

extern const FormatInfo formatInfo[_FORMAT_COUNT];

}  // namespace gfx
