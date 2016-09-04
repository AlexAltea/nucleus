/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/memory/memory.h"
#include "nucleus/gpu/gpu_hash.h"
#include "nucleus/gpu/texture_cache.h"
#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_vp.h"
#include "nucleus/gpu/rsx/rsx_fp.h"
#include "nucleus/gpu/rsx/rsx_texture.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace gpu {
namespace rsx {

// Forward declarations
class RSX;

// RSX Vertex Program attribute
struct rsx_vp_attribute_t {
    bool dirty;            // Flag: Needs to be reloaded and rebinded.
    std::vector<U08> data;  // Holds the loaded and converted data.
    U16 frequency;         // Access frequency of vertex data.
    U08 stride;             // Offset between two consecutive vertices.
    U08 size;               // Count of vertex components
    VertexType type;       // Type of vertex components
    U32 location;          // Location (Local Memory or Main Memory).
    U32 offset;            // Offset at the specified location.
};

struct Surface {
    enum ColorFormat : U08 {
        FORMAT_X1R5G5B5_Z1R5G5B5  = 1,
        FORMAT_X1R5G5B5_O1R5G5B5  = 2,
        FORMAT_R5G6B5             = 3,
        FORMAT_X8R8G8B8_Z8R8G8B8  = 4,
        FORMAT_X8R8G8B8_O8R8G8B8  = 5,
        FORMAT_A8R8G8B8           = 8,
        FORMAT_B8                 = 9,
        FORMAT_G8B8               = 10,
        FORMAT_F_W16Z16Y16X16     = 11,
        FORMAT_F_W32Z32Y32X32     = 12,
        FORMAT_F_X32              = 13,
        FORMAT_X8B8G8R8_Z8B8G8R8  = 14,
        FORMAT_X8B8G8R8_O8B8G8R8  = 15,
        FORMAT_A8B8G8R8           = 16,
    };
    enum DepthStencilFormat : U08 {
        FORMAT_Z16    = 1,
        FORMAT_Z24S8  = 2,
    };

    bool dirty;

    U08 type;
    U08 antialias;
    ColorFormat colorFormat;
    U08 colorTarget;
    U08 colorLocation[4];
    U32 colorOffset[4];
    U32 colorPitch[4];
    DepthStencilFormat depthFormat;
    U08 depthLocation;
    U32 depthOffset;
    U32 depthPitch;
    U16 width;
    U16 height;
    U16 x;
    U16 y;
};

struct rsx_viewport_t {
    bool dirty;

    U16 width;
    U16 height;
    U16 x;
    U16 y;
};

struct rsx_scissor_t {
    bool dirty;

    U16 width;
    U16 height;
    U16 x;
    U16 y;
};

/**
 * Pipeline
 * ========
 * A pipeline represents all the PGRAPH state involved in the creation of a pipeline.
 * This excludes surface offsets and texture samplers.
 */
struct alignas(sizeof(Hash)) Pipeline {
    U32 alpha_func;
    F32 alpha_ref;
    ColorMask color_mask;
    FrontFace front_face;

    // Depth-Stencil state
    bool depth_test_enable;
    bool depth_mask;
    CompareFunc depth_func;
    bool stencil_test_enable;
    bool two_sided_stencil_test_enable;
    U08 stencil_func_mask;
    U08 stencil_mask;
    StencilOp stencil_op_zpass;
    StencilOp stencil_op_zfail;
    StencilOp stencil_op_fail;
    CompareFunc stencil_func;
    StencilOp back_stencil_op_zpass;
    StencilOp back_stencil_op_zfail;
    StencilOp back_stencil_op_fail;
    CompareFunc back_stencil_func;

    // Culling
    bool cull_face_enable;
    CullMode cull_mode;

    // Blending
    bool blend_enable;
    BlendEquation blend_equation_rgb;
    BlendEquation blend_equation_alpha;
    Blend blend_sfactor_rgb;
    Blend blend_sfactor_alpha;
    Blend blend_dfactor_rgb;
    Blend blend_dfactor_alpha;

    // Logic Op
    bool logic_op_enable;
    LogicOp logic_op;

    U64 hash();
};

// RSX's PGRAPH engine (Curie)
class PGRAPH {
    std::shared_ptr<gfx::GraphicsBackend> graphics;
    mem::Memory* memory;
    RSX* rsx;

    gfx::CommandQueue* cmdQueue;
    gfx::CommandBuffer* cmdBuffer;
    gfx::Fence* fence;
    gfx::Heap* heapResources;
    gfx::Heap* heapSamplers;
    gfx::VertexBuffer* vtxTransform;
    gfx::VertexBuffer* vpeConstantMemory;
    gfx::VertexBuffer* vpeInputs[16];

    // Cache
    std::unordered_map<Hash, std::unique_ptr<gfx::Pipeline>> cachePipeline;
    std::unordered_map<Hash, std::unique_ptr<RSXVertexProgram>> cacheVP;
    std::unordered_map<Hash, std::unique_ptr<RSXFragmentProgram>> cacheFP;
    TextureCache cacheTexture;

    // Surface
    std::unordered_map<U32, gfx::Texture*> textures;
    std::unordered_map<U32, gfx::ColorTarget*> colorTargets;
    std::unordered_map<U32, gfx::DepthStencilTarget*> depthStencilTargets;

    // Auxiliary methods
    gfx::ColorTarget* getColorTarget(U32 address);
    gfx::DepthStencilTarget* getDepthStencilTarget(U32 address);

    U64 HashVertexProgram(rsx_vp_instruction_t* program);
    U64 HashFragmentProgram(rsx_fp_instruction_t* program);

    void setSurface();

public:
    Pipeline pipeline;

    // Registers
    U32 clear_color;
    U32 clear_depth;
    U08 clear_stencil;
    U32 semaphore_index;
    U32 vertex_data_base_offset;
    U32 vertex_data_base_index;

    Surface surface;
    rsx_viewport_t viewport;
    rsx_scissor_t scissor;

    bool vertex_transform_dirty;
    V128 viewport_scale;
    V128 viewport_offset;

    // DMA
    U32 dma_report;

    // Textures
    Texture texture[rsx::RSX_MAX_TEXTURES];

    // Vertex Processing Engine
    struct VPE {
        bool dirty_instructions;         // Flag: Instructions need to be recompiled
        bool dirty_constant;             // Flag: Constants need to be reuploaded
        rsx_vp_attribute_t attr[16];     // 16 Vertex Program attributes
        rsx_vp_instruction_t data[512];  // 512 VPE instructions
        V128 constant[468];              // 468 vector constant registers
        U32 constant_load;               // Set through NV4097_SET_TRANSFORM_CONSTANT_LOAD
        U32 load;                        // Set through NV4097_SET_TRANSFORM_PROGRAM_LOAD
        U32 start;                       // Set through NV4097_SET_TRANSFORM_PROGRAM_START
    } vpe;

    // Fragment Program
    bool fp_dirty;                       // Flag: Needs to be recompiled
    U32 fp_location;                     // Location: Local Memory (0) or Main Memory (1)
    U32 fp_offset;                       // Offset at the specified location
    U32 fp_control;                      // Control the performance of the program

    // Constructor
    PGRAPH(std::shared_ptr<gfx::GraphicsBackend> graphics, RSX* rsx, mem::Memory* memory);
    ~PGRAPH();

    // Auxiliary methods
    gfx::Texture* getTexture(U32 address);
    void LoadVertexAttributes(U32 first, U32 count);
    //virtual GLuint GetColorTarget(U32 address);

    // Rendering methods
    void Begin(Primitive mode);
    void End();
    void ClearSurface(U32 mask);
    void DrawArrays(U32 first, U32 count);
    void Enable(U32 prop, U32 enabled);
    void Flip();
};

}  // namespace rsx
}  // namespace gpu
