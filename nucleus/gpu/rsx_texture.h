/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

struct rsx_texture_t {
    // Offset
    u32 offset;        // Offset at the specified location

    // Format
    u8 location;       // Location: Local Memory (0) or Main Memory (1)
    u8 cubemap;
    u8 border;
    u8 dimension;      // Dimension: 1D texture (1), 2D texture (2) or 3D texture (3)
    u8 format;         // Color format and optional swizzle/normalization flags
    u8 mipmap;         // Amount of mipmap leves of the texture

    // Image rectangle
    u16 width;
    u16 height;

    // Border color
    u32 border_color;

    // Address
    u8 wrap_r;
    u8 wrap_s;
    u8 wrap_t;
    u8 anisoBias;
    u8 unsignedRemap;
    u8 signedRemap;
    u8 gamma;
    u8 zfunc;

    // Filter
    union {
        u32 value;

        struct {
            u32 bias : 13;
            u32 conv :  3;
            u32 min  :  8;
            u32 mag  :  4;
            u32 as   :  1;
            u32 rs   :  1;
            u32 gs   :  1;
            u32 bs   :  1;
        };
    } filter;

    // Control 0
    u8 alphakill;
    u8 max_aniso;
    u16 max_lod;
    u16 min_lod;
    u8 enable;         // Texture is enabled: True (1) or False (0)

    // Control 1
    u32 remap;

    // Control 3
    u16 depth;
    u32 pitch;
};

// Texture format
enum {
    // Color value
    RSX_TEXTURE_B8                     = 0x81,
    RSX_TEXTURE_A1R5G5B5               = 0x82,
    RSX_TEXTURE_A4R4G4B4               = 0x83,
    RSX_TEXTURE_R5G6B5                 = 0x84,
    RSX_TEXTURE_A8R8G8B8               = 0x85,
    RSX_TEXTURE_COMPRESSED_DXT1        = 0x86,
    RSX_TEXTURE_COMPRESSED_DXT23       = 0x87,
    RSX_TEXTURE_COMPRESSED_DXT45       = 0x88,
    RSX_TEXTURE_G8B8                   = 0x8B,
    RSX_TEXTURE_R6G5B5                 = 0x8F,
    RSX_TEXTURE_DEPTH24_D8             = 0x90,
    RSX_TEXTURE_DEPTH24_D8_FLOAT       = 0x91,
    RSX_TEXTURE_DEPTH16                = 0x92,
    RSX_TEXTURE_DEPTH16_FLOAT          = 0x93,
    RSX_TEXTURE_X16                    = 0x94,
    RSX_TEXTURE_Y16_X16                = 0x95,
    RSX_TEXTURE_R5G5B5A1               = 0x97,
    RSX_TEXTURE_COMPRESSED_HILO8       = 0x98,
    RSX_TEXTURE_COMPRESSED_HILO_S8     = 0x99,
    RSX_TEXTURE_W16_Z16_Y16_X16_FLOAT  = 0x9A,
    RSX_TEXTURE_W32_Z32_Y32_X32_FLOAT  = 0x9B,
    RSX_TEXTURE_X32_FLOAT              = 0x9C,
    RSX_TEXTURE_D1R5G5B5               = 0x9D,
    RSX_TEXTURE_D8R8G8B8               = 0x9E,
    RSX_TEXTURE_Y16_X16_FLOAT          = 0x9F,
    RSX_TEXTURE_COMPRESSED_B8R8_G8R8   = 0xAD,
    RSX_TEXTURE_COMPRESSED_R8B8_R8G8   = 0xAE,

    // Swizzle flags
    RSX_TEXTURE_SZ  = 0x00,  // Swizzle format
    RSX_TEXTURE_LN  = 0x20,  // Linear format

    // Normalization flags
    RSX_TEXTURE_NR  = 0x00,  // Normalized coordinates
    RSX_TEXTURE_UN  = 0x40,  // Unnormalized coordinates
};

// Texture wrap
enum {
    RSX_TEXTURE_WRAP                        = 1,
    RSX_TEXTURE_MIRROR                      = 2,
    RSX_TEXTURE_CLAMP_TO_EDGE               = 3,
    RSX_TEXTURE_BORDER                      = 4,
    RSX_TEXTURE_CLAMP                       = 5,
    RSX_TEXTURE_MIRROR_ONCE_CLAMP_TO_EDGE   = 6,
    RSX_TEXTURE_MIRROR_ONCE_BORDER          = 7,
    RSX_TEXTURE_MIRROR_ONCE_CLAMP           = 8,
};
