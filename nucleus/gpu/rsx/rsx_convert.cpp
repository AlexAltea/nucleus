/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_convert.h"
#include "nucleus/assert.h"

namespace gpu {
namespace rsx {

gfx::Blend convertBlend(Blend func) {
    switch (func) {
    case RSX_BLEND_ZERO:                      return gfx::BLEND_ZERO;
    case RSX_BLEND_ONE:                       return gfx::BLEND_ONE;
    case RSX_BLEND_SRC_COLOR:                 return gfx::BLEND_SRC_COLOR;
    case RSX_BLEND_ONE_MINUS_SRC_COLOR:       return gfx::BLEND_INV_SRC_COLOR;
    case RSX_BLEND_SRC_ALPHA:                 return gfx::BLEND_SRC_ALPHA;
    case RSX_BLEND_ONE_MINUS_SRC_ALPHA:       return gfx::BLEND_INV_SRC_ALPHA;
    case RSX_BLEND_DST_ALPHA:                 return gfx::BLEND_DEST_ALPHA;
    case RSX_BLEND_ONE_MINUS_DST_ALPHA:       return gfx::BLEND_INV_DEST_ALPHA;
    case RSX_BLEND_DST_COLOR:                 return gfx::BLEND_DEST_COLOR;
    case RSX_BLEND_ONE_MINUS_DST_COLOR:       return gfx::BLEND_INV_DEST_COLOR;
    case RSX_BLEND_SRC_ALPHA_SATURATE:        return gfx::BLEND_SRC_ALPHA_SAT;
    case RSX_BLEND_CONSTANT_COLOR:            return gfx::BLEND_DEST_COLOR;
    case RSX_BLEND_ONE_MINUS_CONSTANT_COLOR:  return gfx::BLEND_INV_DEST_COLOR;

    case RSX_BLEND_CONSTANT_ALPHA:
    case RSX_BLEND_ONE_MINUS_CONSTANT_ALPHA:
        assert_always("Unimplemented");

    default:
        assert_always("Unexpected");
    }
}

gfx::BlendOp convertBlendOp(BlendEquation equation) {
    switch (equation) {
    case RSX_BLEND_EQUATION_FUNC_ADD:               return gfx::BLEND_OP_ADD;
    case RSX_BLEND_EQUATION_MIN:                    return gfx::BLEND_OP_MIN;
    case RSX_BLEND_EQUATION_MAX:                    return gfx::BLEND_OP_MAX;
    case RSX_BLEND_EQUATION_FUNC_SUBTRACT:          return gfx::BLEND_OP_SUBTRACT;
    case RSX_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT:  return gfx::BLEND_OP_REV_SUBTRACT;

    case RSX_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT_SIGNED:
    case RSX_BLEND_EQUATION_FUNC_ADD_SIGNED:
    case RSX_BLEND_EQUATION_FUNC_REVERSE_ADD_SIGNED:
        assert_always("Unimplemented");

    default:
        assert_always("Unexpected");
    }
}

gfx::ColorWriteEnable convertColorMask(ColorMask mask) {
    int colorWriteEnable;
    if (mask & RSX_COLOR_MASK_R)  colorWriteEnable |= gfx::COLOR_WRITE_ENABLE_RED;
    if (mask & RSX_COLOR_MASK_G)  colorWriteEnable |= gfx::COLOR_WRITE_ENABLE_GREEN;
    if (mask & RSX_COLOR_MASK_B)  colorWriteEnable |= gfx::COLOR_WRITE_ENABLE_BLUE;
    if (mask & RSX_COLOR_MASK_A)  colorWriteEnable |= gfx::COLOR_WRITE_ENABLE_ALPHA;
    return static_cast<gfx::ColorWriteEnable>(colorWriteEnable);
}

gfx::LogicOp convertLogicOp(LogicOp op) {
    switch (op) {
    case RSX_LOGIC_OP_CLEAR:          return gfx::LOGIC_OP_CLEAR;
    case RSX_LOGIC_OP_AND:            return gfx::LOGIC_OP_AND;
    case RSX_LOGIC_OP_AND_REVERSE:    return gfx::LOGIC_OP_AND_REVERSE;
    case RSX_LOGIC_OP_COPY:           return gfx::LOGIC_OP_COPY;
    case RSX_LOGIC_OP_AND_INVERTED:   return gfx::LOGIC_OP_AND_INVERTED;
    case RSX_LOGIC_OP_NOOP:           return gfx::LOGIC_OP_NOOP;
    case RSX_LOGIC_OP_XOR:            return gfx::LOGIC_OP_XOR;
    case RSX_LOGIC_OP_OR:             return gfx::LOGIC_OP_OR;
    case RSX_LOGIC_OP_NOR:            return gfx::LOGIC_OP_NOR;
    case RSX_LOGIC_OP_EQUIV:          return gfx::LOGIC_OP_EQUIV;
    case RSX_LOGIC_OP_INVERT:         return gfx::LOGIC_OP_INVERT;
    case RSX_LOGIC_OP_OR_REVERSE:     return gfx::LOGIC_OP_OR_REVERSE;
    case RSX_LOGIC_OP_COPY_INVERTED:  return gfx::LOGIC_OP_COPY_INVERTED;
    case RSX_LOGIC_OP_OR_INVERTED:    return gfx::LOGIC_OP_OR_INVERTED;
    case RSX_LOGIC_OP_NAND:           return gfx::LOGIC_OP_NAND;
    case RSX_LOGIC_OP_SET:            return gfx::LOGIC_OP_SET;
    default:
        assert_always("Unexpected");
    }
}

gfx::PrimitiveTopology convertPrimitiveTopology(Primitive primitive) {
    switch (primitive) {
    case RSX_PRIMITIVE_POINTS:          return gfx::TOPOLOGY_POINT_LIST;
    case RSX_PRIMITIVE_LINES:           return gfx::TOPOLOGY_LINE_LIST;
    case RSX_PRIMITIVE_LINE_LOOP:       return gfx::TOPOLOGY_LINE_LOOP;
    case RSX_PRIMITIVE_LINE_STRIP:      return gfx::TOPOLOGY_LINE_STRIP;
    case RSX_PRIMITIVE_TRIANGLES:       return gfx::TOPOLOGY_TRIANGLE_LIST;
    case RSX_PRIMITIVE_TRIANGLE_STRIP:  return gfx::TOPOLOGY_TRIANGLE_STRIP;
    case RSX_PRIMITIVE_TRIANGLE_FAN:    return gfx::TOPOLOGY_TRIANGLE_LIST;  // TODO: Is this correct?
    case RSX_PRIMITIVE_QUADS:           return gfx::TOPOLOGY_TRIANGLE_LIST;  // TODO: Is this correct?
    case RSX_PRIMITIVE_QUAD_STRIP:      return gfx::TOPOLOGY_TRIANGLE_STRIP; // TODO: Is this correct?
    case RSX_PRIMITIVE_POLYGON:         return gfx::TOPOLOGY_TRIANGLE_LIST;  // TODO: Is this correct?
    default:
        assert_always("Unexpected");
    }
}

gfx::PrimitiveTopologyType convertPrimitiveTopologyType(Primitive primitive) {
    switch (primitive) {
    case RSX_PRIMITIVE_POINTS:          return gfx::TOPOLOGY_TYPE_POINT;
    case RSX_PRIMITIVE_LINES:           return gfx::TOPOLOGY_TYPE_LINE;
    case RSX_PRIMITIVE_LINE_LOOP:       return gfx::TOPOLOGY_TYPE_LINE;
    case RSX_PRIMITIVE_LINE_STRIP:      return gfx::TOPOLOGY_TYPE_LINE;
    case RSX_PRIMITIVE_TRIANGLES:       return gfx::TOPOLOGY_TYPE_TRIANGLE;
    case RSX_PRIMITIVE_TRIANGLE_STRIP:  return gfx::TOPOLOGY_TYPE_TRIANGLE;
    case RSX_PRIMITIVE_TRIANGLE_FAN:    return gfx::TOPOLOGY_TYPE_TRIANGLE;
    case RSX_PRIMITIVE_QUADS:           return gfx::TOPOLOGY_TYPE_TRIANGLE; // TODO: Is this correct?
    case RSX_PRIMITIVE_QUAD_STRIP:      return gfx::TOPOLOGY_TYPE_TRIANGLE; // TODO: Is this correct?
    case RSX_PRIMITIVE_POLYGON:         return gfx::TOPOLOGY_TYPE_TRIANGLE; // TODO: Is this correct?
    default:
        assert_always("Unexpected");
    }
}

gfx::StencilOp convertStencilOp(StencilOp op) {
    switch (op) {
    case RSX_STENCIL_OP_KEEP:       return gfx::STENCIL_OP_KEEP;
    case RSX_STENCIL_OP_REPLACE:    return gfx::STENCIL_OP_REPLACE;
    case RSX_STENCIL_OP_INCR:       return gfx::STENCIL_OP_INCR;
    case RSX_STENCIL_OP_DECR:       return gfx::STENCIL_OP_DECR;
    case RSX_STENCIL_OP_INCR_WRAP:  return gfx::STENCIL_OP_INCR_SAT;
    case RSX_STENCIL_OP_DECR_WRAP:  return gfx::STENCIL_OP_DECR_SAT;
    default:
        assert_always("Unexpected");
    }
}

gfx::Format convertFormat(Surface::ColorFormat format) {
    switch (format) {
    case Surface::FORMAT_R5G6B5:          return gfx::FORMAT_B5G6R5_UNORM;
    case Surface::FORMAT_A8R8G8B8:        return gfx::FORMAT_R8G8B8A8_UNORM;
    case Surface::FORMAT_B8:              return gfx::FORMAT_R8_UNORM;
    case Surface::FORMAT_G8B8:            return gfx::FORMAT_R8G8_UNORM;
    case Surface::FORMAT_F_W16Z16Y16X16:  return gfx::FORMAT_R16G16B16A16_FLOAT;
    case Surface::FORMAT_F_W32Z32Y32X32:  return gfx::FORMAT_R32G32B32A32_FLOAT;
    case Surface::FORMAT_F_X32:           return gfx::FORMAT_R32_FLOAT;
    case Surface::FORMAT_A8B8G8R8:        return gfx::FORMAT_R8G8B8A8_UNORM;

    case Surface::FORMAT_X1R5G5B5_Z1R5G5B5:
    case Surface::FORMAT_X1R5G5B5_O1R5G5B5:
    case Surface::FORMAT_X8R8G8B8_Z8R8G8B8:
    case Surface::FORMAT_X8R8G8B8_O8R8G8B8:
    case Surface::FORMAT_X8B8G8R8_Z8B8G8R8:
    case Surface::FORMAT_X8B8G8R8_O8B8G8R8:
        assert_always("Unimplemented");

    default:
        assert_always("Unexpected");
    }
}

gfx::Format convertFormat(Surface::DepthStencilFormat format) {
    switch (format) {
    case Surface::FORMAT_Z16:    return gfx::FORMAT_D16_UNORM;
    case Surface::FORMAT_Z24S8:  return gfx::FORMAT_D24_UNORM_S8_UINT;

    default:
        assert_always("Unexpected");
    }
}

gfx::Format convertFormat(TextureFormat format) {
    switch (format) {
    case RSX_TEXTURE_B8:                     return gfx::FORMAT_R8_UNORM;
    case RSX_TEXTURE_A1R5G5B5:               return gfx::FORMAT_B5G5R5A1_UNORM;
    case RSX_TEXTURE_A4R4G4B4:               return gfx::FORMAT_B4G4R4A4_UNORM;
    case RSX_TEXTURE_R5G6B5:                 return gfx::FORMAT_B5G6R5_UNORM;
    case RSX_TEXTURE_A8R8G8B8:               return gfx::FORMAT_R8G8B8A8_UNORM;
    case RSX_TEXTURE_COMPRESSED_DXT1:        return gfx::FORMAT_DXT1;
    case RSX_TEXTURE_COMPRESSED_DXT23:       return gfx::FORMAT_DXT23;
    case RSX_TEXTURE_COMPRESSED_DXT45:       return gfx::FORMAT_DXT45;
    case RSX_TEXTURE_G8B8:                   return gfx::FORMAT_G8R8_G8B8_UNORM;
    case RSX_TEXTURE_DEPTH24_D8:             return gfx::FORMAT_R32_UINT;
    case RSX_TEXTURE_DEPTH24_D8_FLOAT:       return gfx::FORMAT_R32_FLOAT;
    case RSX_TEXTURE_DEPTH16:                return gfx::FORMAT_R16_UNORM;
    case RSX_TEXTURE_DEPTH16_FLOAT:          return gfx::FORMAT_R16_FLOAT;
    case RSX_TEXTURE_X16:                    return gfx::FORMAT_R16_UNORM;
    case RSX_TEXTURE_Y16_X16:                return gfx::FORMAT_R16G16_UNORM;
    case RSX_TEXTURE_R5G5B5A1:               return gfx::FORMAT_B5G5R5A1_UNORM;
    case RSX_TEXTURE_W16_Z16_Y16_X16_FLOAT:  return gfx::FORMAT_R16G16B16A16_FLOAT;
    case RSX_TEXTURE_W32_Z32_Y32_X32_FLOAT:  return gfx::FORMAT_R32G32B32A32_FLOAT;
    case RSX_TEXTURE_X32_FLOAT:              return gfx::FORMAT_R32_FLOAT;
    case RSX_TEXTURE_D1R5G5B5:               return gfx::FORMAT_B5G5R5A1_UNORM;
    case RSX_TEXTURE_D8R8G8B8:               return gfx::FORMAT_R8G8B8A8_UNORM;
    case RSX_TEXTURE_COMPRESSED_B8R8_G8R8:   return gfx::FORMAT_G8R8_G8B8_UNORM;
    case RSX_TEXTURE_COMPRESSED_R8B8_R8G8:   return gfx::FORMAT_R8G8_B8G8_UNORM;

    case RSX_TEXTURE_R6G5B5:
    case RSX_TEXTURE_COMPRESSED_HILO8:
    case RSX_TEXTURE_COMPRESSED_HILO_S8:
    case RSX_TEXTURE_Y16_X16_FLOAT:
        assert_always("Unimplemented");

    default:
        assert_always("Unexpected");
    }
}

}  // namespace rsx
}  // namespace gpu
