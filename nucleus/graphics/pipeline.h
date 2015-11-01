/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace graphics {

enum Blend {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_INV_SRC_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_INV_SRC_ALPHA,
    BLEND_DEST_ALPHA,
    BLEND_INV_DEST_ALPHA,
    BLEND_DEST_COLOR,
    BLEND_INV_DEST_COLOR,
    BLEND_SRC_ALPHA_SAT,
    BLEND_BLEND_FACTOR,
    BLEND_INV_BLEND_FACTOR,
    BLEND_SRC1_COLOR,
    BLEND_INV_SRC1_COLOR,
    BLEND_SRC1_ALPHA,
    BLEND_INV_SRC1_ALPHA
};

enum BlendOp {
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REV_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX
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

struct IAState {
};

struct TessState {
};

struct RSState {
};

struct CBState {
    bool enableAlphaToCoverage;
    bool enableDualSourceBlend;
    struct RenderTarget {
        bool enableBlend;
        bool enableLogicOp;
        Blend srcBlend;
        Blend destBlend;
        BlendOp blendOp;
        Blend srcBlendAlpha;
        Blend destBlendAlpha;
        BlendOp blendOpAlpha;
        LogicOp logicOp;
    } renderTarget[8];
};

struct DBState {
};

/**
 * Pipeline shader
 */
class Shader {
};

class PipelineStateDescription {
public:
    Shader vs;  // Vertex Shader
    Shader hs;  // Hull Shader
    Shader ds;  // Domain Shader
    Shader gs;  // Geometry Shader
    Shader ps;  // Pixel Shader

    IAState iaState;      // Input assembler static pipeline state
    TessState tessState;  // Tessellator static pipeline state
    RSState rsState;      // Rasterizer static pipeline state
    CBState cbState;      // Color blender and output static pipeline state
    DBState dbState;      // Depth-stencil static pipeline state
};

class IPipelineState {
public:

    
};

}  // namespace graphics
