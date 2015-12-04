/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

struct VertexBufferDesc {
    U64 size;
};

class VertexBuffer {
public:
    /**
     * Map this buffer into the user address space
     * @return  Address where this vertex buffer was mapped into
     */
    virtual void* map() = 0;

    /**
     * Release the mapping of this buffer from the user address space
     * @return  True on success 
     */
    virtual bool unmap() = 0;
};

}  // namespace gfx
