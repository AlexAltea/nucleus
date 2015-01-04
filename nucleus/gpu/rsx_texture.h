/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

struct rsx_texture_t {
    u8 format;    // Color format and optional swizzle/normalization flags
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
    RSX_TEXTURE_NR	= 0x00,  // Normalized coordinates
	RSX_TEXTURE_UN	= 0x40,  // Unnormalized coordinates
};
