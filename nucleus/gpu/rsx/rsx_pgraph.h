/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/memory/memory.h"
#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_vp.h"
#include "nucleus/gpu/rsx/rsx_fp.h"
#include "nucleus/gpu/rsx/rsx_texture.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace gpu {

// Forward declarations
class RSX;

// RSX Vertex Program attribute
struct rsx_vp_attribute_t {
    bool dirty;             // Flag: Needs to be reloaded and rebinded.
    std::vector<U8> data;  // Holds the loaded and converted data.
    U16 frequency;         // Access frequency of vertex data.
    U8 stride;             // Offset between two consecutive vertices.
    U8 size;               // Coordinates per vertex.
    U8 type;               // Format (S1, F, SF, UB, S32K, CMP, UB256).
    U32 location;          // Location (Local Memory or Main Memory).
    U32 offset;            // Offset at the specified location.
};

struct rsx_surface_t {
    bool dirty;

    U8 type;
    U8 antialias;
    U8 colorFormat;
    U8 colorTarget;
    U8 colorLocation[4];
    U32 colorOffset[4];
    U32 colorPitch[4];
    U8 depthFormat;
    U8 depthLocation;
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

// RSX's PGRAPH engine (Curie)
class PGRAPH {
    std::shared_ptr<gfx::IBackend> graphics;
    mem::Memory* memory;
    RSX* rsx;


    gfx::CommandBuffer* cmdBuffer;

    // Cache
    std::unordered_map<U64, RSXVertexProgram> cacheVP;
    std::unordered_map<U64, RSXFragmentProgram> cacheFP;

    // Surface
    std::unordered_map<U32, gfx::ColorTarget*> colorTargets;
    std::unordered_map<U32, gfx::DepthStencilTarget*> depthStencilTargets;
    /*GLuint framebuffer;
    std::unordered_map<U32, GLuint> colorTargets;
    std::unordered_map<U32, GLuint> depthTargets;*/

    // Auxiliary methods
    gfx::ColorTarget* getColorTarget(U32 address);
    gfx::DepthStencilTarget* getDepthTarget(U32 address);

    void setSurface();
    void setViewport();
    void setPipeline();

public:
    // Registers
    U32 alpha_func;
    F32 alpha_ref;
    U16 blend_sfactor_rgb;
    U16 blend_sfactor_alpha;
    U16 blend_dfactor_rgb;
    U16 blend_dfactor_alpha;
    U32 semaphore_index;
    U32 vertex_data_base_offset;
    U32 vertex_data_base_index;
    U32 vertex_primitive;

    rsx_surface_t surface;
    rsx_viewport_t viewport;

    // DMA
    U32 dma_report;

    // Textures
    rsx_texture_t texture[RSX_MAX_TEXTURES];

    // Vertex Processing Engine
    struct VPE {
        bool dirty;                      // Flag: Needs to be recompiled
        rsx_vp_attribute_t attr[16];     // 16 Vertex Program attributes
        rsx_vp_instruction_t data[512];  // 512 VPE instructions
        rsx_vp_constant_t constant[468]; // 468 vector constant registers
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
    PGRAPH(std::shared_ptr<gfx::IBackend> graphics, RSX* rsx, mem::Memory* memory);
    ~PGRAPH();

    // Hashing methods
    U64 HashTexture();
    U64 HashVertexProgram(rsx_vp_instruction_t* program);
    U64 HashFragmentProgram(rsx_fp_instruction_t* program);

    // Auxiliary methods
    void LoadVertexAttributes(U32 first, U32 count);
    //virtual GLuint GetColorTarget(U32 address);

    // Rendering methods
    void AlphaFunc(U32 func, F32 ref);
    void Begin(U32 mode);
    void BindVertexAttributes();
    void ClearColor(U8 a, U8 r, U8 g, U8 b);
    void ClearDepth(U32 value);
    void ClearStencil(U32 value);
    void ClearSurface(U32 mask);
    void ColorMask(bool a, bool r, bool g, bool b);
    void DepthFunc(U32 func);
    void DrawArrays(U32 first, U32 count);
    void Enable(U32 prop, U32 enabled);
    void End();
    void Flip();
    void SurfaceColorTarget(U32 target);
    void UnbindVertexAttributes();
};

}  // namespace gpu
