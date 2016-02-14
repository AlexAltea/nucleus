/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "format.h"

namespace gfx {

const FormatInfo formatInfo[_FORMAT_COUNT] = {
#define FORMAT(name, bytesPerPixel) { FORMAT_##name, bytesPerPixel },
#include "format.inl"
#undef FORMAT
};

}  // namespace gfx
