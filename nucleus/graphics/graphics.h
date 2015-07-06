/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace graphics {

enum PrimitiveTopology {
    TOPOLOGY_POINT_LIST,
    TOPOLOGY_LINE_LIST,
    TOPOLOGY_LINE_STRIP,
    TOPOLOGY_TRIANGLE_LIST,
    TOPOLOGY_TRIANGLE_STRIP,
    TOPOLOGY_QUAD_LIST,
    TOPOLOGY_QUAD_STRIP,
};

class IBackend {

    // Command queue management
    virtual void CreateCommandQueue() = 0;
    virtual void CommandQueueSubmit() = 0;

    // Command buffer management
    virtual void CreateCommandBuffer() = 0;
    virtual void BeginCommandBuffer() = 0;
    virtual void EndCommandBuffer() = 0;
    virtual void ResetCommandBuffer() = 0;

    virtual void CmdBindPipeline() = 0;
    virtual void CmdDraw() = 0;
    virtual void CmdDrawIndexed() = 0;
    virtual void CmdDrawIndirect() = 0;
    virtual void CmdDrawIndexedIndirect() = 0;

    // Pipeline management
    virtual void CreatePipeline() = 0;

    // Shader management
    virtual void CreateShader() = 0;
};

}  // namespace graphics
