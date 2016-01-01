/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace gfx {

enum Primitive {
};

enum PrimitiveTopology {
    TOPOLOGY_POINT_LIST,
    TOPOLOGY_LINE_LIST,
    TOPOLOGY_LINE_STRIP,
    TOPOLOGY_TRIANGLE_LIST,
    TOPOLOGY_TRIANGLE_STRIP,
    TOPOLOGY_QUAD_LIST,
    TOPOLOGY_QUAD_STRIP,
};

enum PrimitiveTopologyType {
    TOPOLOGY_TYPE_POINT,
    TOPOLOGY_TYPE_LINE,
    TOPOLOGY_TYPE_TRIANGLE,
    TOPOLOGY_TYPE_PATCH,
};

}  // namespace gfx
