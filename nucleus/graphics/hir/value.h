/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/opcodes.h"
#include "nucleus/graphics/hir/type.h"

namespace gfx {
namespace hir {

// Forward declarations
class Module;
class Function;
class Instruction;

enum ValueFlags {
    VALUE_IS_CONSTANT  = (1 << 0),
    VALUE_IS_ARGUMENT  = (1 << 1),
    VALUE_IS_INPUT     = (1 << 2),
    VALUE_IS_OUTPUT    = (1 << 3),
    VALUE_IS_BUILTIN   = (1 << 4),
};

enum ValueBuiltin {
    VALUE_BUILTIN_POSITION,
    VALUE_BUILTIN_POINTSIZE,
    VALUE_BUILTIN_CLIPDISTANCE,
    VALUE_BUILTIN_CULLDISTANCE,
    VALUE_BUILTIN_FRAGCOORD,
    VALUE_BUILTIN_POINTCOORD,
    VALUE_BUILTIN_FRAGDEPTH,
};

class Value {
    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    union Parent {
        Module* module;
        Function* function;
        Instruction* instruction;
    } parent;

    union Constant {
        S16 i16;
        S32 i32;
        F32 f16; // TODO
        F32 f32;
        V128 v128;
    } constant;

    ValueBuiltin builtin;

    // Counter of instructions using this value as source
    U32 usage;

    // Flags
    U32 flags;

    Type type;

    // Get ID of this value
    S32 getId();
    void setId(S32 id);

    // Type properties
    bool isTypeInteger() const;
    bool isTypeFloat() const;
    bool isTypeVector() const;

    // Constant properties
    bool isConstant() const;
    bool isConstantFalse() const;
    bool isConstantTrue() const;
    bool isConstantZero() const;
    bool isConstantNonzero() const;
    bool isConstantEqual(Constant other) const;

    // Constants setters
    void setConstantI16(U16 c);
    void setConstantI32(U32 c);
    void setConstantF16(F32 c); // TODO
    void setConstantF32(F32 c);
    void setConstantV128(V128 c);
};

}  // namespace hir
}  // namespace gfx
