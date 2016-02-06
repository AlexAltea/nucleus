/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_convert.h"
#include "nucleus/assert.h"

namespace gfx {
namespace direct3d12 {

D3D12_BLEND convertBlend(gfx::Blend blend) {
    switch (blend) {
    case BLEND_ZERO:              return D3D12_BLEND_ZERO;
    case BLEND_ONE:               return D3D12_BLEND_ONE;
    case BLEND_SRC_COLOR:         return D3D12_BLEND_SRC_COLOR;
    case BLEND_INV_SRC_COLOR:     return D3D12_BLEND_INV_SRC_COLOR;
    case BLEND_SRC_ALPHA:         return D3D12_BLEND_SRC_ALPHA;
    case BLEND_INV_SRC_ALPHA:     return D3D12_BLEND_INV_SRC_ALPHA;
    case BLEND_DEST_ALPHA:        return D3D12_BLEND_DEST_ALPHA;
    case BLEND_INV_DEST_ALPHA:    return D3D12_BLEND_INV_DEST_ALPHA;
    case BLEND_DEST_COLOR:        return D3D12_BLEND_DEST_COLOR;
    case BLEND_INV_DEST_COLOR:    return D3D12_BLEND_INV_DEST_COLOR;
    case BLEND_SRC_ALPHA_SAT:     return D3D12_BLEND_SRC_ALPHA_SAT;
    case BLEND_BLEND_FACTOR:      return D3D12_BLEND_BLEND_FACTOR;
    case BLEND_INV_BLEND_FACTOR:  return D3D12_BLEND_INV_BLEND_FACTOR;
    case BLEND_SRC1_COLOR:        return D3D12_BLEND_SRC1_COLOR;
    case BLEND_INV_SRC1_COLOR:    return D3D12_BLEND_INV_SRC1_COLOR;
    case BLEND_SRC1_ALPHA:        return D3D12_BLEND_SRC1_ALPHA;
    case BLEND_INV_SRC1_ALPHA:    return D3D12_BLEND_INV_SRC1_ALPHA;
    default:
        assert_always("Unimplemented case");
        return D3D12_BLEND_ZERO;
    }
}

D3D12_BLEND_OP convertBlendOp(gfx::BlendOp blendOp) {
    switch (blendOp) {
    case BLEND_OP_ADD:           return D3D12_BLEND_OP_ADD;
    case BLEND_OP_SUBTRACT:      return D3D12_BLEND_OP_SUBTRACT;
    case BLEND_OP_REV_SUBTRACT:  return D3D12_BLEND_OP_REV_SUBTRACT;
    case BLEND_OP_MIN:           return D3D12_BLEND_OP_MIN;
    case BLEND_OP_MAX:           return D3D12_BLEND_OP_MAX;
    default:
        assert_always("Unimplemented case");
        return D3D12_BLEND_OP_ADD;
    }
}

D3D12_COMPARISON_FUNC convertComparisonFunc(gfx::ComparisonFunc comparisonFunc) {
    switch (comparisonFunc) {
    case COMPARISON_FUNC_NEVER:          return D3D12_COMPARISON_FUNC_NEVER;
    case COMPARISON_FUNC_LESS:           return D3D12_COMPARISON_FUNC_LESS;
    case COMPARISON_FUNC_EQUAL:          return D3D12_COMPARISON_FUNC_EQUAL;
    case COMPARISON_FUNC_LESS_EQUAL:     return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case COMPARISON_FUNC_GREATER:        return D3D12_COMPARISON_FUNC_GREATER;
    case COMPARISON_FUNC_NOT_EQUAL:      return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case COMPARISON_FUNC_GREATER_EQUAL:  return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case COMPARISON_FUNC_ALWAYS:         return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        assert_always("Unimplemented case");
        return D3D12_COMPARISON_FUNC_NEVER;
    }
}

D3D12_FILTER convertFilter(gfx::Filter filter) {
    switch (filter) {
    case FILTER_MIN_MAG_MIP_POINT:                return D3D12_FILTER_MIN_MAG_MIP_POINT;
    case FILTER_MIN_MAG_POINT_MIP_LINEAR:         return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:   return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case FILTER_MIN_POINT_MAG_MIP_LINEAR:         return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case FILTER_MIN_LINEAR_MAG_MIP_POINT:         return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:  return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case FILTER_MIN_MAG_LINEAR_MIP_POINT:         return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case FILTER_MIN_MAG_MIP_LINEAR:               return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    default:
        assert_always("Unimplemented case");
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
}

DXGI_FORMAT convertFormat(gfx::Format format) {
    switch (format) {
    case FORMAT_R32_FLOAT:           return DXGI_FORMAT_R32_FLOAT;
    case FORMAT_R32G32_FLOAT:        return DXGI_FORMAT_R32G32_FLOAT;
    case FORMAT_R32G32B32_FLOAT:     return DXGI_FORMAT_R32G32B32_FLOAT;
    case FORMAT_R32G32B32A32_FLOAT:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case FORMAT_R8_UNORM:            return DXGI_FORMAT_R8_UNORM;
    case FORMAT_R8G8B8A8_UNORM:      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case FORMAT_D16_UNORM:           return DXGI_FORMAT_D16_UNORM;
    case FORMAT_D24_UNORM_S8_UINT:   return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
        assert_always("Unimplemented case");
        return DXGI_FORMAT_UNKNOWN;
    }
}

D3D12_INPUT_CLASSIFICATION convertInputClassification(gfx::InputClassification inputClassification) {
    switch (inputClassification) {
    case INPUT_CLASSIFICATION_PER_VERTEX:    return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    case INPUT_CLASSIFICATION_PER_INSTANCE:  return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    default:
        assert_always("Unimplemented case");
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    }
}

D3D12_LOGIC_OP convertLogicOp(gfx::LogicOp logicOp) {
    switch (logicOp) {
    case LOGIC_OP_COPY:           return D3D12_LOGIC_OP_COPY;
    case LOGIC_OP_CLEAR:          return D3D12_LOGIC_OP_CLEAR;
    case LOGIC_OP_AND:            return D3D12_LOGIC_OP_AND;
    case LOGIC_OP_AND_REVERSE:    return D3D12_LOGIC_OP_AND_REVERSE;
    case LOGIC_OP_AND_INVERTED:   return D3D12_LOGIC_OP_AND_INVERTED;
    case LOGIC_OP_NOOP:           return D3D12_LOGIC_OP_NOOP;
    case LOGIC_OP_XOR:            return D3D12_LOGIC_OP_XOR;
    case LOGIC_OP_OR:             return D3D12_LOGIC_OP_OR;
    case LOGIC_OP_NOR:            return D3D12_LOGIC_OP_NOR;
    case LOGIC_OP_EQUIV:          return D3D12_LOGIC_OP_EQUIV;
    case LOGIC_OP_INVERT:         return D3D12_LOGIC_OP_INVERT;
    case LOGIC_OP_OR_REVERSE:     return D3D12_LOGIC_OP_OR_REVERSE;
    case LOGIC_OP_COPY_INVERTED:  return D3D12_LOGIC_OP_COPY_INVERTED;
    case LOGIC_OP_OR_INVERTED:    return D3D12_LOGIC_OP_OR_INVERTED;
    case LOGIC_OP_NAND:           return D3D12_LOGIC_OP_NAND;
    case LOGIC_OP_SET:            return D3D12_LOGIC_OP_SET;
    default:
        assert_always("Unimplemented case");
        return D3D12_LOGIC_OP_NOOP;
    }
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE convertPrimitiveTopologyType(gfx::PrimitiveTopology primitiveTopology) {
    switch (primitiveTopology) {
    case TOPOLOGY_POINT_LIST:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case TOPOLOGY_LINE_LIST:       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case TOPOLOGY_LINE_STRIP:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case TOPOLOGY_TRIANGLE_LIST:   return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case TOPOLOGY_TRIANGLE_STRIP:  return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case TOPOLOGY_QUAD_LIST:       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case TOPOLOGY_QUAD_STRIP:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default:
        assert_always("Unimplemented case");
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

D3D12_RESOURCE_STATES convertResourceState(gfx::ResourceState resourceState) {
    switch (resourceState) {
    case RESOURCE_STATE_PRESENT:       return D3D12_RESOURCE_STATE_PRESENT;
    case RESOURCE_STATE_COLOR_TARGET:  return D3D12_RESOURCE_STATE_RENDER_TARGET;
    default:
        assert_always("Unimplemented case");
        return D3D12_RESOURCE_STATE_COMMON;
    }
}

D3D12_TEXTURE_ADDRESS_MODE convertTextureAddressMode(gfx::TextureAddress address) {
    switch (address) {
    case TEXTURE_ADDRESS_WRAP:         return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case TEXTURE_ADDRESS_MIRROR:       return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case TEXTURE_ADDRESS_CLAMP:        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case TEXTURE_ADDRESS_BORDER:       return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    case TEXTURE_ADDRESS_MIRROR_ONCE:  return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
    default:
        assert_always("Unimplemented case");
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

D3D12_RESOURCE_FLAGS convertTextureFlags(gfx::TextureFlags flags) {
    D3D12_RESOURCE_FLAGS d3dFlags = D3D12_RESOURCE_FLAG_NONE;
    if (flags & TEXTURE_FLAG_COLOR_TARGET)         d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (flags & TEXTURE_FLAG_DEPTHSTENCIL_TARGET)  d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    return d3dFlags;
}

}  // namespace direct3d12
}  // namespace gfx
