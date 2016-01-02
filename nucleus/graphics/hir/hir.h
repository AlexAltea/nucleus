/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {
namespace hir {

/**
 * High-level Intermediate Representation
 * ======================================
 * Since the Vulkan backend has priority over backends and its shading and computing
 * intermediate representation is flexible and powerful enough for all current goals,
 * this HIR mimics a subset of SPIR-V. This could however change in the future.
 *
 * References:
 *  - SPIR-V Specification. Version 1.00, Revision 2. November 14, 2015.
 */

using Literal = U32;

}  // namespace hir
}  // namespace gfx
