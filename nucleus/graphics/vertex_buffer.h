/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/format.h"
#include "nucleus/graphics/resource.h"

namespace gfx {

struct VertexBufferDesc {
    U64 size;
    bool cbvCreation;
};

class VertexBuffer : public virtual Resource {
public:
    virtual ~VertexBuffer() = default;
};

}  // namespace gfx
