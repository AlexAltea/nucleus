/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/hir/hir.h"

#include <vector>

namespace gfx {
namespace hir {

/*enum Type {
    TYPE_VOID,
    TYPE_I16,
    TYPE_I32,
    TYPE_F16,
    TYPE_F32,
    TYPE_V128,
};*/

struct Type {
    Literal id = 0;
};

struct TypeInt : Type {
    Literal width;
    Literal signedness;
};

struct TypeVector : Type {
    Type* componentType;
    Literal componentCount;
};

struct TypeStruct : Type {
    std::vector<Type*> members;
};

struct TypeFunction : Type {
    Type* returnType;
    std::vector<Type*> parameterTypes;
};

}  // namespace hir
}  // namespace gfx
