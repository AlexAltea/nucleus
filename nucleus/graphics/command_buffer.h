/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/primitive.h"
#include "nucleus/graphics/resource.h"
#include "nucleus/graphics/target.h"
#include "nucleus/graphics/vertex_buffer.h"
#include "nucleus/graphics/viewport.h"
#include "nucleus/graphics/rectangle.h"

namespace gfx {

class CommandBuffer {
public:
    /**
     * Reset the command buffer, reverting it back to the state of a new command buffer
     * @return  True on success
     */
    virtual bool reset() = 0;

    /**
     * Finalize the command buffer, making it ready to be sent to a command queue
     * @return  True on success
     */
    virtual bool finalize() = 0;

    /**
     * Bind a graphics pipeline to the current command buffer state
     * @param[in]  pipeline  Pipeline state
     */
    virtual void cmdBindPipeline(Pipeline* pipeline) = 0;

    /**
     * Pushes a command to clear a color buffer
     * @param[in]  target      Color buffer to be cleared
     * @param[in]  colorValue  The 4-component array of the RGBA value to clear with
     */
    virtual void cmdClearColor(ColorTarget* target, const F32* colorValue) = 0;

    /**
     * Pushes a command to clear a depth-stencil buffer
     * @param[in]  target        Depth-stencil buffer to be cleared
     * @param[in]  depthValue    The value to clear the depth buffer with
     * @param[in]  stencilValue  The value to clear the stencil buffer with
     */
    virtual void cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) = 0;

    /**
     * Pushes a command to draw instanced, non-indexed geometry
     * @param[in]  firstVertex    Offset to the first vertex
     * @param[in]  vertexCount    Number of vertices per instance to draw
     * @param[in]  firstInstance  Offset to the first instance
     * @param[in]  instanceCount  Number of instances to draw
     */
    virtual void cmdDraw(U32 firstVertex, U32 vertexCount, U32 firstInstance, U32 instanceCount) = 0;

    /**
     * Pushes a command to draw instanced, indexed geometry
     * @param[in]  firstIndex     Offset to the first index
     * @param[in]  indexCount     Number of indices per instance to draw
     * @param[in]  vertexOffset   Vertex offset to be added to each vertex index
     * @param[in]  firstInstance  Offset to the first instance
     * @param[in]  instanceCount  Number of instances to draw
     */
    virtual void cmdDrawIndexed(U32 firstIndex, U32 indexCount, U32 vertexOffset, U32 firstInstance, U32 instanceCount) = 0;

    /**
     * Bind a texture to a pipeline sampler
     * @param[in]  buffers  Uniform buffers to bind
     * @param[in]  texture  Textures to bind
     */
    virtual void cmdSetDescriptors(const std::vector<VertexBuffer*>& buffers, const std::vector<Texture*>& textures) = 0;

    /**
     * Pushes a command to set the vertex buffer for an input slots
     * @param[in]  index           Slot index
     * @param[in]  vtxBufferCount  Amount of slot indices to set
     * @param[in]  vtxBuffer       Array of vertex buffers
     * @param[in]  offsets         Offsets
     * @param[in]  strides         Strides
     */
    virtual void cmdSetVertexBuffers(U32 index, U32 vtxBufferCount, VertexBuffer** vtxBuffer, U32* offsets, U32* strides) = 0;

    /**
     * Pushes a command to set the primitive topology used for drawing
     * @param[in]  topology  Primitive topology
     */
    virtual void cmdSetPrimitiveTopology(PrimitiveTopology topology) = 0;

    /**
     * Pushes a command to set the color targets and depth-stencil target for rendering
     * @param[in]  colorCount          Length of color targets array
     * @param[in]  colorTargets        Color targets array
     * @param[in]  depthStencilTarget  Depth-stencil target (optional)
     */
    virtual void cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) = 0;

    /**
     * Pushes a command to set the viewports
     * @param[in]  viewportsCount  Length of viewports array
     * @param[in]  viewports       Viewports array
     */
    virtual void cmdSetViewports(U32 viewportsCount, const Viewport* viewports) = 0;

    /**
     * Pushes a command to set the scissor rectangles
     * @param[in]  scissorsCount  Length of scissor rectangles array
     * @param[in]  scissors       Scissor rectangles array
     */
    virtual void cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) = 0;

    /**
     * Pushes a command to trigger a resource barrier
     * @param[in]  barrierCount  Length of the resource barrier array
     * @param[in]  barriers      Resource barrier array
     */
    virtual void cmdResourceBarrier(U32 barrierCount, const ResourceBarrier* barriers) = 0;
};

}  // namespace gfx
