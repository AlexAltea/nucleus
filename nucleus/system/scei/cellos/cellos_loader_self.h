/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/system/elf64_loader.h"
#include "nucleus/system/scei/cellos/lv2/sys_prx.h"

#include <string>
#include <vector>


// Forward declaration
namespace sys { class LV2; }

namespace sys {
namespace scei {
namespace cellos {

// ELF ABI
enum {
    ELFOSABI_CELL_LV2 = 0x66,
};

// Segment types
enum {
    // Cell OS Lv-2 (OS) specific
    PT_PROC_PARAM   = 0x60000001,
    PT_PRX_PARAM    = 0x60000002,

    // Cell B.E. (CPU) specific
    PT_SCE_PPURELA  = 0x700000A4,
};

// Segment flags
enum {
    // Cell OS Lv-2 (OS) specific
    PF_SPU_X   = 0x00100000,  // SPU execute
    PF_SPU_W   = 0x00200000,  // SPU write
    PF_SPU_R   = 0x00400000,  // SPU read
    PF_RSX_X   = 0x01000000,  // RSX execute
    PF_RSX_W   = 0x02000000,  // RSX write
    PF_RSX_R   = 0x04000000,  // RSX read
};

// Relocations
enum {
    R_PPC64_NONE       = 0, // Nothing
    R_PPC64_ADDR32     = 1, // u32:  (S + A)
    R_PPC64_ADDR24     = 2, // u24:  (S + A) >> 2
    R_PPC64_ADDR16     = 3, // u16:  (S + A)
    R_PPC64_ADDR16_LO  = 4, // u16:  LO(S + A)
    R_PPC64_ADDR16_HI  = 5, // u16:  HI(S + A)
    R_PPC64_ADDR16_HA  = 6, // u16:  HA(S + A)
    R_PPC64_ADDR14     = 7, // u14:  (S + A) >> 2
};

// SELF structs
struct SceHeader {
    BE<U32> magic;
    BE<U32> hver;
    BE<U16> flags;
    BE<U16> type;
    BE<U32> meta;
    BE<U64> hsize;
    BE<U64> esize;
};

struct SelfHeader {
    BE<U64> htype;
    BE<U64> appinfooff;
    BE<U64> elfoff;
    BE<U64> phdroff;
    BE<U64> shdroff;
    BE<U64> secinfoff;
    BE<U64> sceveroff;
    BE<U64> controloff;
    BE<U64> controlsize;
    BE<U64> pad;
};

struct AppInfo {
    BE<U64> authid;
    BE<U32> vendor_id;
    BE<U32> self_type;
    BE<U64> version;
    BE<U64> padding;
};

struct MetadataInfo {
    U08 key[0x10];
    U08 key_pad[0x10];
    U08 iv[0x10];
    U08 iv_pad[0x10];
};

struct MetadataHeader {
    BE<U64> signature_input_length;
    BE<U32> unknown1;
    BE<U32> section_count;
    BE<U32> key_count;
    BE<U32> opt_header_size;
    BE<U32> unknown2;
    BE<U32> unknown3;
};

struct MetadataSectionHeader {
    BE<U64> data_offset;
    BE<U64> data_size;
    BE<U32> type;
    BE<U32> program_idx;
    BE<U32> hashed;
    BE<U32> sha1_idx;
    BE<U32> encrypted;
    BE<U32> key_idx;
    BE<U32> iv_idx;
    BE<U32> compressed;
};

struct ControlInfo {
    BE<U32> type;
    BE<U32> size;
    BE<U64> next;

    union {
        struct {  // Type 1 (0x30 bytes)
            BE<U32> ctrl_flag1;
            BE<U32> unknown1;
            BE<U32> unknown2;
            BE<U32> unknown3;
            BE<U32> unknown4;
            BE<U32> unknown5;
            BE<U32> unknown6;
            BE<U32> unknown7;
        } control_flags;

        struct {  // Type 2 (0x30 bytes)
            BE<U08> digest[20];
            BE<U64> unknown;
        } file_digest40;

        struct {  // Type 2 (0x40 bytes)
            BE<U08> digest1[20];
            BE<U08> digest2[20];
            BE<U64> unknown;
        } file_digest30;

        struct {  // Type 3 (0x90 bytes)
            BE<U32> magic;
            BE<U32> unknown1;
            BE<U32> license;
            BE<U32> type;
            BE<U08> content_id[48];
            BE<U08> digest[16];
            BE<U08> invdigest[16];
            BE<U08> xordigest[16];
            BE<U64> unknown2;
            BE<U64> unknown3;
        } npdrm;
    };
};

/**
 * SELF Loader
 * ============
 * Parses SELF files.
 * @tparam  E  Endianness
 */
class SELFLoader : public Elf64Loader<BE> {
protected:
    std::vector<Byte> elf;  // Holds the decrypted executable
    std::vector<Byte> self; // Holds the encrypted executable

                            // Decrypts the Metadata Info and Headers of a SELF file
    bool decryptMetadata();

    // Returns the size of the ELF file after the SELF decryption/decompression
    // by using accessing the SELF's EHDR and decrypted Metadata headers.
    U32 getDecryptedElfSize();

    void process_seg_custom_os(System* sys, const Phdr& phdr, std::vector<Byte>& data);
    void process_seg_custom_proc(System* sys, const Phdr& phdr, std::vector<Byte>& data);

public:
    bool open(fs::File* file);
    bool open(LV2* lv2, const std::string& path);
    bool load_elf(LV2* lv2);
    bool load_prx(LV2* lv2, sys::sys_prx_t& prx);
    bool decrypt();
    void close();

    U16 getMachine();
    U64 getEntry();
};

}  // namespace cellos
}  // namespace scei
}  // namespace sys
