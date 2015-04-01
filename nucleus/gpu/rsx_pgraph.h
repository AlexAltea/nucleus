/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/rsx_enum.h"
#include "nucleus/gpu/rsx_vp.h"
#include "nucleus/gpu/rsx_fp.h"
#include "nucleus/gpu/rsx_texture.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

#include <vector>

// RSX Vertex Program attribute
struct rsx_vp_attribute_t {
    bool dirty;             // Flag: Needs to be reloaded and rebinded.
    std::vector<u8> data;   // Holds the loaded and converted data.
    u16 frequency;          // Access frequency of vertex data.
    u8 stride;              // Offset between two consecutive vertices.
    u8 size;                // Coordinates per vertex.
    u8 type;                // Format (S1, F, SF, UB, S32K, CMP, UB256).
    u32 location;           // Location (Local Memory or Main Memory).
    u32 offset;             // Offset at the specified location.
};

struct rsx_surface_t {
    bool dirty;

    u8 type;
    u8 antialias;
    u8 colorFormat;
    u8 colorTarget;
    u8 colorLocation[4];
    u32 colorOffset[4];
    u32 colorPitch[4];
    u8 depthFormat;
    u8 depthLocation;
    u32 depthOffset;
    u32 depthPitch;
    u16 width;
    u16 height;
    u16 x;
    u16 y;
};

struct rsx_viewport_t {
    bool dirty;

    u16 width;
    u16 height;
    u16 x;
    u16 y;
};

// RSX's PGRAPH engine (Curie)
class PGRAPH {
public:
    // Registers
    u32 alpha_func;
    u32 alpha_ref;
    u16 blend_sfactor_rgb;
    u16 blend_sfactor_alpha;
    u16 blend_dfactor_rgb;
    u16 blend_dfactor_alpha;
    u32 semaphore_index;
    u32 vertex_data_base_offset;
    u32 vertex_data_base_index;
    u32 vertex_primitive;

    rsx_surface_t surface;
    rsx_viewport_t viewport;

    // DMA
    u32 dma_report;

    // Textures
    rsx_texture_t texture[RSX_MAX_TEXTURES];

    // Vertex Processing Engine
    struct VPE {
        bool dirty;                      // Flag: Needs to be recompiled
        rsx_vp_attribute_t attr[16];     // 16 Vertex Program attributes
        rsx_vp_instruction_t data[512];  // 512 VPE instructions
        rsx_vp_constant_t constant[468]; // 468 vector constant registers
        u32 constant_load;               // Set through NV4097_SET_TRANSFORM_CONSTANT_LOAD
        u32 load;                        // Set through NV4097_SET_TRANSFORM_PROGRAM_LOAD
        u32 start;                       // Set through NV4097_SET_TRANSFORM_PROGRAM_START
    } vpe;

    // Fragment Program
    bool fp_dirty;                      // Flag: Needs to be recompiled
    u32 fp_location;                    // Location: Local Memory (0) or Main Memory (1)
    u32 fp_offset;                      // Offset at the specified location
    u32 fp_control;                     // Control the performance of the program

    // Hashing methods
    u64 HashTexture();
    u64 HashVertexProgram(rsx_vp_instruction_t* program);
    u64 HashFragmentProgram(rsx_fp_instruction_t* program);

    // Auxiliary methods
    void LoadVertexAttributes(u32 first, u32 count);
    virtual GLuint GetColorTarget(u32 address)=0;

    // Rendering methods
    virtual void AlphaFunc(u32 func, f32 ref)=0;
    virtual void Begin(u32 mode)=0;
    virtual void BindVertexAttributes()=0;
    virtual void ClearColor(u8 a, u8 r, u8 g, u8 b)=0;
    virtual void ClearDepth(u32 value)=0;
    virtual void ClearStencil(u32 value)=0;
    virtual void ClearSurface(u32 mask)=0;
    virtual void ColorMask(bool a, bool r, bool g, bool b)=0;
    virtual void DepthFunc(u32 func)=0;
    virtual void DrawArrays(u32 first, u32 count)=0;
    virtual void Enable(u32 prop, u32 enabled)=0;
    virtual void End()=0;
    virtual void Flip()=0;
    virtual void SurfaceColorTarget(u32 target)=0;
    virtual void UnbindVertexAttributes()=0;
};
