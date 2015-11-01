/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/target.h"

namespace gfx {

class ICommandBuffer {
public:
    /**
     * Reset the command buffer, reverting it back to the state of a new command buffer
     * @return                    True on success
     */
    virtual bool reset() = 0;

    /**
     * Bind a graphics pipeline to the current command buffer state
     * @param[in]  pipeline       Pipeline state
     */
    virtual void cmdBindPipeline(IPipelineState* pipeline) = 0;

    /**
     * Pushes a command to clear a color buffer
     * @param[in]  target         Color buffer to be cleared
     * @param[in]  colorValue     The 4-component array of the RGBA value to clear with
     */
    virtual void cmdClearColor(IColorTarget* target, const F32* colorValue) = 0;

    /**
     * Pushes a command to clear a depth-stencil buffer
     * @param[in]  target         Depth-stencil buffer to be cleared
     * @param[in]  depthValue     The value to clear the depth buffer with
     * @param[in]  stencilValue   The value to clear the stencil buffer with
     */
    virtual void cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) = 0;

    /**
     * Pushes a command to draw instanced, non-indexed geometry
     */
    virtual void cmdDraw() = 0;

    /**
     * Pushes a command to draw instanced, indexed geometry
     * @param[in]  firstIndex     Offset to the first index
     * @param[in]  indexCount     Number of indices per instance to draw
     * @param[in]  vertexOffset   Vertex offset to be added to each vertex index
     * @param[in]  firstInstance  Offset to the first instance
     * @param[in]  instanceCount  Number of instances to draw
     */
    virtual void cmdDrawIndexed() = 0;

    /**
     * Pushes a command to draw instanced, non-indexed geometry from GPU-stored data
     */
    virtual void cmdDrawIndirect() = 0;

    /**
     * Pushes a command to draw instanced, indexed geometry from GPU-stored data
     */
    virtual void cmdDrawIndexedIndirect() = 0;
};

}  // namespace gfx
