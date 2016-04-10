/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/system/scei/cellos/lv2/sys_process.h"
#include "nucleus/system/scei/cellos/lv2/sys_prx.h"

#include <string>
#include <vector>

/**
 * ELF64 Header constants
 */
enum ELFType {
    ET_NONE = 0,  // No file type
    ET_REL  = 1,  // Relocatable object file
    ET_EXEC = 2,  // Executable file
    ET_DYN  = 3,  // Shared object file
    ET_CORE = 4,  // Core file
};

enum ELFMachine {
    EM_UNKNOWN,
    EM_MIPS   = 0x08,
    EM_PPC64  = 0x15,
    EM_SPU    = 0x17,
    EM_ARM    = 0x28,
    EM_X86_64 = 0x3E,
};

// Segment types
enum {
    // Common segments
    PT_LOAD     = 0x1,
    PT_DYNAMIC  = 0x2,
    PT_INTERP   = 0x3,
    PT_NOTE     = 0x4,
    PT_SHLIB    = 0x5,
    PT_PHDR     = 0x6,
    PT_TLS      = 0x7,

    // Cell OS Lv-2 (OS) specific segments
    PT_PROC_PARAM   = 0x60000001,
    PT_PRX_PARAM    = 0x60000002,

    // Cell B.E. (CPU) specific segments
    PT_SCE_PPURELA  = 0x700000A4,
};

// Segment flags
enum {
    // Common flags
    PF_X       = 0x00000001,  // Segment is executable (by PPU)
    PF_W       = 0x00000002,  // Segment is writable (by PPU)
    PF_R       = 0x00000004,  // Segment is readable (by PPU)

    // Cell OS Lv-2 (OS) specific flags
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
struct SceHeader
{
    BE<U32> magic;
    BE<U32> hver;
    BE<U16> flags;
    BE<U16> type;
    BE<U32> meta;
    BE<U64> hsize;
    BE<U64> esize;
};

struct SelfHeader
{
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

struct AppInfo
{
    BE<U64> authid;
    BE<U32> vendor_id;
    BE<U32> self_type;
    BE<U64> version;
    BE<U64> padding;
};

struct MetadataInfo
{
    U8 key[0x10];
    U8 key_pad[0x10];
    U8 iv[0x10];
    U8 iv_pad[0x10];
};

struct MetadataHeader
{
    BE<U64> signature_input_length;
    BE<U32> unknown1;
    BE<U32> section_count;
    BE<U32> key_count;
    BE<U32> opt_header_size;
    BE<U32> unknown2;
    BE<U32> unknown3;
};

struct MetadataSectionHeader
{
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

struct ControlInfo
{
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
            U8 digest[20];
            U64 unknown;
        } file_digest40;

        struct {  // Type 2 (0x40 bytes)
          U8 digest1[20];
          U8 digest2[20];
          BE<U64> unknown;
        } file_digest30;

        struct {  // Type 3 (0x90 bytes)
            BE<U32> magic;
            BE<U32> unknown1;
            BE<U32> license;
            BE<U32> type;
            U8 content_id[48];
            U8 digest[16];
            U8 invdigest[16];
            U8 xordigest[16];
            BE<U64> unknown2;
            BE<U64> unknown3;
        } npdrm;
    };
};

// ELF64 structs
struct Elf64_Ehdr
{
    BE<U32> magic;
    BE<U8>  elf_class;
    BE<U8>  data;
    BE<U8>  curver;
    BE<U8>  os_abi;
    BE<U64> abi_ver;
    BE<U16> type;
    BE<U16> machine;
    BE<U32> version;
    BE<U64> entry;
    BE<U64> phoff;
    BE<U64> shoff;
    BE<U32> flags;
    BE<U16> ehsize;
    BE<U16> phentsize;
    BE<U16> phnum;
    BE<U16> shentsize;
    BE<U16> shnum;
    BE<U16> shstrndx;
};

struct Elf64_Phdr
{
    BE<U32> type;
    BE<U32> flags;
    BE<U64> offset;
    BE<U64> vaddr;
    BE<U64> paddr;
    BE<U64> filesz;
    BE<U64> memsz;
    BE<U64> align;
};

struct Elf64_Shdr
{
    BE<U32> name;
    BE<U32> type;
    BE<U64> flags;
    BE<U64> addr;
    BE<U64> offset;
    BE<U64> size;
    BE<U32> link;
    BE<U32> info;
    BE<U64> addralign;
    BE<U64> entsize;
};

class SELFLoader
{
    std::vector<char> elf;  // Holds the decrypted executable
    std::vector<char> self; // Holds the encrypted executable

    // Decrypts the Metadata Info and Headers of a SELF file
    bool decryptMetadata();

    // Returns the size of the ELF file after the SELF decryption/decompression
    // by using accessing the SELF's EHDR and decrypted Metadata headers.
    U32 getDecryptedElfSize();

public:
    bool open(fs::File* file);
    bool open(const std::string& path);
    bool load_elf(sys::sys_process_t& proc);
    bool load_prx(sys::sys_prx_t& prx);
    bool decrypt();
    void close();

    U16 getMachine();
    U64 getEntry();
};
