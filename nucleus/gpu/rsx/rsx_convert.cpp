/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_convert.h"
#include "nucleus/assert.h"

namespace gpu {
namespace rsx {

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
