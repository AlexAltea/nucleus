/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_pgraph.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_methods.h"

namespace gpu {

U64 PGRAPH::HashTexture()
{
    return 0;
}

U64 PGRAPH::HashVertexProgram(rsx_vp_instruction_t* program)
{
    // 64-bit Fowler/Noll/Vo FNV-1a hash code
    U64 hash = 0xCBF29CE484222325ULL;
    do {
        hash ^= program->dword[0];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        hash ^= program->dword[1];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
    } while (!(program++)->end);
    return hash;
}

U64 PGRAPH::HashFragmentProgram(rsx_fp_instruction_t* program)
{
    // 64-bit Fowler/Noll/Vo FNV-1a hash code
    bool end = false;
    U64 hash = 0xCBF29CE484222325ULL;
    do {
        hash ^= program->dword[0];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        hash ^= program->dword[1];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        end = ((program++)->word[0] >> 8) & 0x1; // NOTE: We can't acces program->end directly, since words require byte swapping
    } while (!end);
    return hash;
}

void PGRAPH::LoadVertexAttributes(U32 first, U32 count)
{
    // Bytes per vertex coordinate. Index is given by attribute::type.
    static const U32 vertexTypeSize[] = {
        0, 2, 4, 2, 1, 2, 4, 1
    };

    for (auto& attr : vpe.attr) {
        if (!attr.size) {
            continue;
        }

        // Get vertex buffer address
        U32 addr;
        if (attr.location == RSX_LOCATION_LOCAL) {
            addr = nucleus.memory(SEG_RSX_LOCAL_MEMORY).getBaseAddr() + attr.offset;
        } else {
            addr = nucleus.rsx.get_ea(attr.offset);
        }

        const U32 typeSize = vertexTypeSize[attr.type];
        attr.data.resize(count * attr.size * typeSize);

        // Copy data per vertex
        for (U32 i = 0; i < count; i++) {
            U32 src = addr + vertex_data_base_offset + attr.stride * (first + i + vertex_data_base_index);
            void* dst = &attr.data[i * attr.size * typeSize];

            switch (typeSize) {
            case 1:
                for (size_t j = 0; j < attr.size; j++) {
                    ((U8*)dst)[j] = nucleus.memory.read8(src + 1*j);
                }
                break;
            case 2:
                for (size_t j = 0; j < attr.size; j++) {
                    ((U16*)dst)[j] = nucleus.memory.read16(src + 2*j);
                }
                break;
            case 4:
                for (size_t j = 0; j < attr.size; j++) {
                    ((U32*)dst)[j] = nucleus.memory.read32(src + 4*j);
                }
                break;
            }
        }
    }
}

/*
PGRAPH::PGRAPH()
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

PGRAPH::~PGRAPH() 
{
    glDeleteFramebuffers(1, &framebuffer);
}
*/

void PGRAPH::SetColorTarget(U32 address, U8 attachment)
{
    /*// Generate a texture to hold the color buffer
    if (colorTargets.find(address) == colorTargets.end()) {
        GLuint colorTexture;
        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface.width, surface.height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        colorTargets[address] = colorTexture;
    }
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, colorTargets[address], 0);
    checkRendererError("SetColorTarget");*/
}

void PGRAPH::SetDepthTarget(U32 address)
{
    /*// Generate a texture to hold the depth buffer
    if (depthTargets.find(address) == depthTargets.end()) {
        GLuint depthTexture;
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, surface.width, surface.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTargets[address] = depthTexture;
    }
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTargets[address], 0);
    checkRendererError("SetDepthTarget");*/
}

/*GLuint PGRAPH::GetColorTarget(U32 address)
{
    if (colorTargets.find(address) == colorTargets.end()) {
        return 0;
    }
    return colorTargets[address];
}*/

/**
 * PGRAPH methods
 */

void PGRAPH::AlphaFunc(U32 func, F32 ref)
{
    /*glAlphaFunc(func, ref);
    checkRendererError("AlphaFunc");*/
}

// NOTE: RSX doesn't know how big the vertex buffer is, but OpenGL requires this information
// to copy the data to the host GPU. Therefore, LoadVertexAttributes needs to be called.
void PGRAPH::BindVertexAttributes()
{
    /*// Indices are attr.type
    static const GLenum vertexType[] = {
        0, GL_SHORT, GL_FLOAT, GL_HALF_FLOAT, GL_UNSIGNED_BYTE, GL_SHORT, GL_FLOAT, GL_UNSIGNED_BYTE
    };
    static const GLboolean vertexNormalized[] = {
        0, GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (int index = 0; index < 16; index++) {
        const auto& attr = vpe.attr[index];
        if (attr.size) {
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, attr.data.size(), attr.data.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, attr.size, vertexType[attr.type], vertexNormalized[attr.type], 0, 0);
        }
    }*/
}

void PGRAPH::Begin(U32 mode)
{
    vertex_primitive = mode;
}

void PGRAPH::ClearColor(U8 a, U8 r, U8 g, U8 b)
{
    /*glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    checkRendererError("ClearColor");*/
}

void PGRAPH::ClearDepth(U32 value)
{
    /*glClearDepth(value / (float)0xFFFFFF);
    checkRendererError("ClearDepth");*/
}

void PGRAPH::ClearStencil(U32 value)
{
    /*glClearStencil(value);
    checkRendererError("ClearStencil");*/
}

void PGRAPH::ClearSurface(U32 mask)
{
    /*// Set mask
    GLbitfield clearMask = 0;
    if (mask & 0x01) clearMask |= GL_DEPTH_BUFFER_BIT;
    if (mask & 0x02) clearMask |= GL_STENCIL_BUFFER_BIT;
    if (mask & 0xF0) clearMask |= GL_COLOR_BUFFER_BIT;

    glClear(clearMask);
    checkRendererError("ClearSurface");*/
}

void PGRAPH::ColorMask(bool a, bool r, bool g, bool b)
{
    /*glColorMask(r, g, b, a);
    checkRendererError("ColorMask");*/
}

void PGRAPH::DepthFunc(U32 func)
{
    /*glDepthFunc(func);
    checkRendererError("DepthFunc");*/
}

void PGRAPH::DrawArrays(U32 first, U32 count)
{
    /*// State
    glBlendFuncSeparate(blend_sfactor_rgb, blend_dfactor_rgb, blend_sfactor_alpha, blend_dfactor_alpha);

    // Surface
    if (surface.dirty) {
        surface.dirty = false;
        switch (surface.colorTarget) {
        case RSX_SURFACE_TARGET_NONE:    
            break;

        case RSX_SURFACE_TARGET_0:
            SetColorTarget(surface.colorOffset[0], 0);
            break;

        case RSX_SURFACE_TARGET_1:
            SetColorTarget(surface.colorOffset[1], 1);
            break;

        case RSX_SURFACE_TARGET_MRT1:
            SetColorTarget(surface.colorOffset[0], 0);
            SetColorTarget(surface.colorOffset[1], 1);
            break;

        case RSX_SURFACE_TARGET_MRT2:
            SetColorTarget(surface.colorOffset[0], 0);
            SetColorTarget(surface.colorOffset[1], 1);
            SetColorTarget(surface.colorOffset[2], 2);
            break;

        case RSX_SURFACE_TARGET_MRT3:
            SetColorTarget(surface.colorOffset[0], 0);
            SetColorTarget(surface.colorOffset[1], 1);
            SetColorTarget(surface.colorOffset[2], 2);
            SetColorTarget(surface.colorOffset[3], 3);
            break;
        }
        SetDepthTarget(surface.depthOffset);

        GLuint fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fbStatus != GL_FRAMEBUFFER_COMPLETE) {
            logger.error(LOG_GPU, "PGRAPH::DrawArrays: Framebuffer is not complete (0x%X)", fbStatus);
        }
    }

    // Viewport
    if (viewport.dirty) {
        viewport.dirty = false;
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    // Shaders
    auto vp_data = &vpe.data[vpe.start];
    auto vp_hash = HashVertexProgram(vp_data);
    if (cache_vp.find(vp_hash) == cache_vp.end()) {
        OpenGLVertexProgram vp;
        vp.decompile(vp_data);
        vp.compile();
        cache_vp[vp_hash] = vp;
    }

    auto fp_data = nucleus.memory.ptr<rsx_fp_instruction_t>((fp_location ? nucleus.rsx.get_ea(0x0) : 0xC0000000) + fp_offset);
    auto fp_hash = HashFragmentProgram(fp_data);
    if (cache_fp.find(fp_hash) == cache_fp.end()) {
        OpenGLFragmentProgram fp;
        fp.decompile(fp_data);
        fp.compile();
        cache_fp[fp_hash] = fp;
    }

    // Link, validate and use program
    GLuint id = glCreateProgram();
    glAttachShader(id, cache_vp[vp_hash].id);
    glAttachShader(id, cache_fp[fp_hash].id);
    glLinkProgram(id);
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        logger.error(LOG_GPU, "PGRAPH::DrawArrays: Can't link program");
    }
    glGetProgramiv(id, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        logger.error(LOG_GPU, "PGRAPH::DrawArrays: Can't validate program");
    }
    glUseProgram(id);

    // Upload VP constants
    for (U32 i = 0; i < 468; i++) {
        auto& constant = vpe.constant[i];
        if (constant.dirty) {
            GLint loc = glGetUniformLocation(id, format("c[%d]", i).c_str());
            glUniform4f(loc, constant.x, constant.y, constant.z, constant.w);
            constant.dirty = false;
        }
    }

    // Bind textures
    for (U32 i = 0; i < RSX_MAX_TEXTURES; i++) {
        const auto& tex = texture[i];
        if (tex.enable) {
            GLuint tid;
            glActiveTexture(GL_TEXTURE0 + i);
            glGenTextures(1, &tid);
            glBindTexture(GL_TEXTURE_2D, tid);

            // Init texture
            void* texaddr = nucleus.memory.ptr<void>((tex.location ? nucleus.rsx.get_ea(0x0) : 0xC0000000) + tex.offset);
            switch (tex.format & ~RSX_TEXTURE_LN & ~RSX_TEXTURE_UN) {
            case RSX_TEXTURE_B8:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_BLUE, GL_UNSIGNED_BYTE, texaddr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_BLUE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_BLUE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
                break;
            case RSX_TEXTURE_A8R8G8B8:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, texaddr);
                break;
            default:
                logger.error(LOG_GPU, "Unsupported texture format (%d)", tex.format);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    }

    GLenum mode = vertex_primitive - 1;
    glDrawArrays(mode, first, count);
    checkRendererError("DrawArrays");*/
}

void PGRAPH::Enable(U32 prop, U32 enabled)
{
    /*switch (prop) {
    case NV4097_SET_DITHER_ENABLE:
        enabled ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
        break;

    case NV4097_SET_ALPHA_TEST_ENABLE:
        enabled ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
        break;

    case NV4097_SET_STENCIL_TEST_ENABLE:
        enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        break;

    case NV4097_SET_DEPTH_TEST_ENABLE:
        enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        break;

    case NV4097_SET_CULL_FACE_ENABLE:
        enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        break;

    case NV4097_SET_BLEND_ENABLE:
        enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        break;

    case NV4097_SET_POLY_OFFSET_FILL_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
        break;

    case NV4097_SET_POLY_OFFSET_LINE_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
        break;

    case NV4097_SET_POLY_OFFSET_POINT_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
        break;

    case NV4097_SET_LOGIC_OP_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LOGIC_OP) : glDisable(GL_LOGIC_OP);
        break;

    case NV4097_SET_SPECULAR_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
        break;

    case NV4097_SET_LINE_SMOOTH_ENABLE:
        enabled ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
        break;

    case NV4097_SET_POLY_SMOOTH_ENABLE:
        enabled ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
        break;
    }
    checkRendererError("Enable");*/
}

void PGRAPH::End()
{
    vertex_primitive = 0;
}

void PGRAPH::Flip()
{
}

void PGRAPH::SurfaceColorTarget(U32 target)
{
    /*GLenum bufs[4];
    switch (target) {
    case RSX_SURFACE_TARGET_NONE:
        glDrawBuffers(0, nullptr);
        break;

    case RSX_SURFACE_TARGET_0:
        bufs[0] = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, bufs);
        break;

    case RSX_SURFACE_TARGET_1:
        bufs[0] = GL_COLOR_ATTACHMENT1;
        glDrawBuffers(1, bufs);
        break;

    case RSX_SURFACE_TARGET_MRT1:
        bufs[0] = GL_COLOR_ATTACHMENT0;
        bufs[1] = GL_COLOR_ATTACHMENT1;
        glDrawBuffers(2, bufs);
        break;

    case RSX_SURFACE_TARGET_MRT2:
        bufs[0] = GL_COLOR_ATTACHMENT0;
        bufs[1] = GL_COLOR_ATTACHMENT1;
        bufs[2] = GL_COLOR_ATTACHMENT2;
        glDrawBuffers(3, bufs);
        break;

    case RSX_SURFACE_TARGET_MRT3:
        bufs[0] = GL_COLOR_ATTACHMENT0;
        bufs[1] = GL_COLOR_ATTACHMENT1;
        bufs[2] = GL_COLOR_ATTACHMENT2;
        bufs[3] = GL_COLOR_ATTACHMENT3;
        glDrawBuffers(4, bufs);
        break;
    }
    checkRendererError("SurfaceColorTarget");*/
}

void PGRAPH::UnbindVertexAttributes()
{
    /*for (int index = 0; index < 16; index++) {
        glDisableVertexAttribArray(index);
        checkRendererError("UnbindVertexAttributes");
    }
    glBindVertexArray(0);*/
}

}  // namespace gpu
