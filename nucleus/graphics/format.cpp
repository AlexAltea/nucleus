/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "format.h"

namespace gfx {

const FormatInfo formatInfo[_FORMAT_COUNT] = {
    { FORMAT_R32G32, 8 },
    { FORMAT_R32G32B32, 12 },
    { FORMAT_R32G32B32A32, 16 },
    { FORMAT_R8G8B8A8_UNORM, 4 },
};

}  // namespace gfx
