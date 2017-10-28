/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#define LV2_SYSCALL(name, ...) \
    U64 name(LV2& kernel, __VA_ARGS__)

namespace sys {

// Forward declarations
class LV2;

}  // namespace sys
