/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gpu {

using Hash = U64;

/**
 * Hash a structure using of arbitrary type.
 * This uses the 64-bit Fowler/Noll/Vo FNV-1a hash code.
 * @tparam     Type    Type of the object to hash
 * @param[in]  object  Object to hash of the specified type
 * @return             Hash of the object
 */
template <typename Type>
Hash hashStruct(const Type& object) {
    static_assert((sizeof(Type) % sizeof(Hash)) == 0, "Unimplemented support for arbitrarily-sized types");

    // Initial hash value
    Hash hash = 0xCBF29CE484222325ULL;
    for (Size offset = 0; offset < sizeof(Type); offset += 8) {
        hash ^= *reinterpret_cast<const U64*>(reinterpret_cast<const char*>(&object) + offset);
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
    }
    return hash;
}

/**
 * Hash an arbitrarily-long array of arbitrary type given its base address and
 * the signal to stop hashing. This uses the 64-bit Fowler/Noll/Vo FNV-1a hash code.
 * @tparam     Component  Component type of the array to hash
 * @param[in]  base       Pointer to the first element of the array
 * @return                Hash of the object
 */
template <typename Component>
Hash hashArray(const Component* base) {
    static_assert((sizeof(Component) % sizeof(Hash)) == 0, "Unimplemented support for arbitrarily-sized components");

    // Initial hash value
    Hash hash = 0xCBF29CE484222325ULL;
    return hash;
}

}  // namespace gpu
