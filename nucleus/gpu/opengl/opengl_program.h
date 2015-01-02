/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

// Transform the 4-bit mask of the [x,y,z,w] values into the GLSL equivalent
const char* get_mask(u8 maskValue);

// Transform a the 2-bit swizzle masks for [x,y,z,w] packed as a u8 into the GLSL equivalent
std::string get_swizzling(u8 swizzleValue);
