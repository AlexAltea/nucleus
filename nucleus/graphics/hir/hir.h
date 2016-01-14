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

enum StorageClass {
    INPUT = 1,
    UNIFORM = 2,
    OUTPUT = 3,
};

enum Opcode {
    OP_TYPE_VOID = 19,
    OP_TYPE_BOOL = 20,
    OP_TYPE_INT = 21,
    OP_TYPE_FLOAT = 22,
    OP_TYPE_VECTOR = 23,
    OP_TYPE_MATRIX = 24,
    OP_TYPE_IMAGE = 25,
    OP_TYPE_SAMPLER = 26,
    OP_TYPE_SAMPLED_IMAGE = 27,
    OP_TYPE_ARRAY = 28,
    OP_TYPE_STRUCT = 30,
    OP_TYPE_POINTER = 32,
    OP_TYPE_FUNCTION = 33,
};

}  // namespace hir
}  // namespace gfx
