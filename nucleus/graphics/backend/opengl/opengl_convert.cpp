/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_convert.h"
#include "nucleus/assert.h"

namespace gfx {
namespace opengl {

GLenum convertBlend(gfx::Blend blend) {
    switch (blend) {
    case BLEND_ZERO:              return GL_ZERO;
    case BLEND_ONE:               return GL_ONE;
    case BLEND_SRC_COLOR:         return GL_SRC_COLOR;
    case BLEND_INV_SRC_COLOR:     return GL_ONE_MINUS_SRC_COLOR;
    case BLEND_SRC_ALPHA:         return GL_SRC_ALPHA;
    case BLEND_INV_SRC_ALPHA:     return GL_ONE_MINUS_SRC_ALPHA;
    case BLEND_SRC1_COLOR:        return GL_SRC1_COLOR;
    case BLEND_INV_SRC1_COLOR:    return GL_ONE_MINUS_SRC1_COLOR;
    case BLEND_SRC1_ALPHA:        return GL_SRC1_ALPHA;
    case BLEND_INV_SRC1_ALPHA:    return GL_ONE_MINUS_SRC1_ALPHA;
    default:
        assert_always("Unimplemented case");
        return GL_ZERO;
    }
}

GLenum convertBlendOp(gfx::BlendOp blendOp) {
    switch (blendOp) {
    case BLEND_OP_ADD:           return GL_FUNC_ADD;
    case BLEND_OP_SUBTRACT:      return GL_FUNC_SUBTRACT;
    case BLEND_OP_REV_SUBTRACT:  return GL_FUNC_REVERSE_SUBTRACT;
    case BLEND_OP_MIN:           return GL_MIN;
    case BLEND_OP_MAX:           return GL_MAX;
    default:
        assert_always("Unimplemented case");
        return GL_FUNC_ADD;
    }
}

GLenum convertLogicOp(gfx::LogicOp logicOp) {
    switch (logicOp) {
    case LOGIC_OP_COPY:           return GL_COPY;
    case LOGIC_OP_CLEAR:          return GL_CLEAR;
    case LOGIC_OP_AND:            return GL_AND;
    case LOGIC_OP_AND_REVERSE:    return GL_AND_REVERSE;
    case LOGIC_OP_AND_INVERTED:   return GL_AND_INVERTED;
    case LOGIC_OP_NOOP:           return GL_NOOP;
    case LOGIC_OP_XOR:            return GL_XOR;
    case LOGIC_OP_OR:             return GL_OR;
    case LOGIC_OP_NOR:            return GL_NOR;
    case LOGIC_OP_EQUIV:          return GL_EQUIV;
    case LOGIC_OP_INVERT:         return GL_INVERT;
    case LOGIC_OP_OR_REVERSE:     return GL_OR_REVERSE;
    case LOGIC_OP_COPY_INVERTED:  return GL_COPY_INVERTED;
    case LOGIC_OP_OR_INVERTED:    return GL_OR_INVERTED;
    case LOGIC_OP_NAND:           return GL_NAND;
    case LOGIC_OP_SET:            return GL_SET;
    default:
        assert_always("Unimplemented case");
        return GL_NOOP;
    }
}

}  // namespace opengl
}  // namespace gfx
