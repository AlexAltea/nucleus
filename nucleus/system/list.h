/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#ifdef NUCLEUS_PLATFORM_PS3
#include "nucleus/system/scei/cellos/lv1.h"
#include "nucleus/system/scei/cellos/lv2.h"
#endif
#ifdef NUCLEUS_PLATFORM_PS4
#include "nucleus/system/scei/orbisos/orbisos.h"
#endif

namespace sys {

// Shorthands
#ifdef NUCLEUS_PLATFORM_PS3
using LV2 = sys::LV2;
#endif
#ifdef NUCLEUS_PLATFORM_PS4
using OrbisOS = sys::scei::orbis::OrbisOS;
#endif

}  // namespace sys
