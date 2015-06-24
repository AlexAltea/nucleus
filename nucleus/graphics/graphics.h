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

enum LogicOp {
    LOGIC_OP_COPY,
    LOGIC_OP_CLEAR,
    LOGIC_OP_AND,
    LOGIC_OP_AND_REVERSE,
    LOGIC_OP_AND_INVERTED,
    LOGIC_OP_NOOP,
    LOGIC_OP_XOR,
    LOGIC_OP_OR,
    LOGIC_OP_NOR,
    LOGIC_OP_EQUIV,
    LOGIC_OP_INVERT,
    LOGIC_OP_OR_REVERSE,
    LOGIC_OP_COPY_INVERTED,
    LOGIC_OP_OR_INVERTED,
    LOGIC_OP_NAND,
    LOGIC_OP_SET,
};

class IBackend {

    // Queue management
    virtual void QueueSubmit() = 0;

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
};

}  // graphics
