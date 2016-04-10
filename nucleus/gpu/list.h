/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#ifdef NUCLEUS_PLATFORM_PS3
#include "nucleus/gpu/rsx/rsx.h"
#endif
#ifdef NUCLEUS_PLATFORM_PS4
//#include "nucleus/gpu/r10xx/r10xx.h"
#endif

namespace gpu {

// Shorthands
#ifdef NUCLEUS_PLATFORM_PS3
using RSX = rsx::RSX;
#endif
#ifdef NUCLEUS_PLATFORM_PS4
//using R10XX = r10xx::R10XX;
#endif

}  // namespace gpu
