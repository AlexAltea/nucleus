/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "value.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/assert.h"

namespace gfx {
namespace hir {

S32 Value::getId() {
    if (id < 0) {
        Function* parFunction;
        if (flags & VALUE_IS_ARGUMENT) {
            parFunction = parent.function;
        } else {
            parFunction = parent.instruction->parent->parent;
        }
        id = parFunction->valueIdCounter++;
    }
    return id;
}

void Value::setId(S32 newId) {
    id = newId;
}

// Type properties
bool Value::isTypeInteger() const {
    return type == TYPE_I16 || type == TYPE_I32;
}
bool Value::isTypeFloat() const {
    return type == TYPE_F16 || type == TYPE_F32;
}
bool Value::isTypeVector() const {
    return type == TYPE_V128;
}

// Constant properties
bool Value::isConstant() const {
    return (flags & VALUE_IS_CONSTANT);
}

bool Value::isConstantFalse() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I16:  return !constant.i16;
    case TYPE_I32:  return !constant.i32;
    case TYPE_F16:  return !constant.f16;
    case TYPE_F32:  return !constant.f32;
    case TYPE_V128: assert_always("Unimplemented Value::isConstantFalse for TYPE_V128"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

bool Value::isConstantTrue() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I16:  return !!constant.i16;
    case TYPE_I32:  return !!constant.i32;
    case TYPE_F16:  return !!constant.f16;
    case TYPE_F32:  return !!constant.f32;
    case TYPE_V128: assert_always("Unimplemented Value::isConstantTrue for TYPE_V128"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

bool Value::isConstantZero() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I16:  return (constant.i16 == 0);
    case TYPE_I32:  return (constant.i32 == 0);
    case TYPE_F16:  return (constant.f16 == 0);
    case TYPE_F32:  return (constant.f32 == 0);
    case TYPE_V128: assert_always("Unimplemented Value::isConstantZero for TYPE_V128"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

// Constants setters
void Value::setConstantI16(U16 c) {
    constant.i16 = c;
    type = TYPE_I16;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantI32(U32 c) {
    constant.i32 = c;
    type = TYPE_I32;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantF16(F32 c) {
    // TODO
}
void Value::setConstantF32(F32 c) {
    constant.f32 = c;
    type = TYPE_F32;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantV128(V128 c) {
    constant.v128 = c;
    type = TYPE_V128;
    flags |= VALUE_IS_CONSTANT;
}

}  // namespace hir
}  // namespace gfx
