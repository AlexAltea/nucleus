/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <vector>

namespace sys {

enum PKGRevision : U16 {
    PKG_REVISION_DEBUG   = 0x0000,
    PKG_REVISION_RELEASE = 0x8000,
};

enum PKGType : U16 {
    PKG_TYPE_PS3 = 0x0001,  // PlayStation 3
    PKG_TYPE_PSP = 0x0002,  // PlayStation Portable
};

struct PKGHeader {
    BE<U32> magic;                  // Magic
    BE<U16> pkg_revision;           // Revision
    BE<U16> pkg_type;               // Type
    BE<U32> pkg_info_offset;        // Info offset
    BE<U32> pkg_info_count;         // Info count
    BE<U32> header_size;            // Header size (usually 0xC0)
    BE<U32> item_count;             // Files and folders in the encrypted data
    BE<U64> total_size;             // Total PKG file size
    BE<U64> data_offset;            // Encrypted data offset
    BE<U64> data_size;              // Encrypted data size
    S8 contentid[48];               // Content ID (similar to "XX####-XXXX#####_##-XXXXXXXXXXXX####")
    S8 digest[16];                  // SHA1 hash from files and attributes
    S8 klicensee[16];               // AES-128-CTR IV
    S8 header_cmac_hash[16];        // CMAC OMAC hash of [0x00, 0x7F]
    S8 header_npdrm_signature[40];  // Header NPDRM ECDSA (R_sig, S_sig)
    S8 header_sha1_hash[8];         // Last 8 bytes of SHA1 of [0x00, 0x7F]
};

struct PKGEntry {
    BE<U32> name_offset;
    BE<U32> name_size;
    BE<U64> data_offset;
    BE<U64> data_size;
    BE<U32> type;
    BE<U32> padding;
};

class PKGLoader {
public:
    bool install(const std::string& path);
};

}  // namespace sys
