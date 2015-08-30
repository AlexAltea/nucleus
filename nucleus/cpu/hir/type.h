/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace hir {

enum Type {
    TYPE_VOID,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_F32,
    TYPE_F64,
    TYPE_V128,
	TYPE_V256,

    TYPE_PTR = TYPE_I64
};

}  // namespace hir
}  // namespace cpu
