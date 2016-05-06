/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/gpu_hash.h"

#include <deque>
#include <unordered_map>

// Forward declarations
namespace gfx { class Texture; }

namespace gpu {

/**
 * Texture Cache
 * =============
 * Utility for the emulated guest GPU to upload and cache textures.
 *
 * Implementation:
 * - Textures are hashed and stored into a map, additionally the hash is
 *   inserted into a ring buffer which resizes to accomodate all hashes
 *   such that their collective texture size remains under the limit
 *   specified by the user.
 * - When a hash is removed from the ring buffer, the corresponding
 *   texture cache entry is deleted and thus freed from the host GPU memory.
 */
class TextureCache {
    // Ring buffer of hashes
    std::deque<Hash> hash;

    // Holds each cached texture 
    std::unordered_map<Hash, gfx::Texture*> textureCache;

    Size curSize;
    Size maxSize;

public:
    TextureCache(Size maxSize) :
        curSize(0), maxSize(maxSize) {}

    /**
     * Tries to find the specified texture in the cache
     * @param[in]  addr  Texture address
     * @param[in]  size  Texture size
     * @return           Pointer to the GPU texture object if found, otherwise nullptr.
     */
    gfx::Texture* get(const Byte* addr, Size size);
};
    
}  // namespace gpu
