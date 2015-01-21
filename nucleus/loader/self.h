/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/syscalls/lv2/sys_prx.h"

#include <string>

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
    PF_X            = (1 << 0),  // Segment is executable
    PF_W            = (1 << 1),  // Segment is writable
    PF_R            = (1 << 2),  // Segment is readable

    // Cell OS Lv-2 (OS) specific flags
    PF_UNK_00400000 = (1 << 22),
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
    be_t<u32> magic;
    be_t<u32> hver;
    be_t<u16> flags;
    be_t<u16> type;
    be_t<u32> meta;
    be_t<u64> hsize;
    be_t<u64> esize;
};

struct SelfHeader
{
    be_t<u64> htype;
    be_t<u64> appinfooff;
    be_t<u64> elfoff;
    be_t<u64> phdroff;
    be_t<u64> shdroff;
    be_t<u64> secinfoff;
    be_t<u64> sceveroff;
    be_t<u64> controloff;
    be_t<u64> controlsize;
    be_t<u64> pad;
};

struct AppInfo
{
    be_t<u64> authid;
    be_t<u32> vendor_id;
    be_t<u32> self_type;
    be_t<u64> version;
    be_t<u64> padding;
};

struct MetadataInfo
{
    u8 key[0x10];
    u8 key_pad[0x10];
    u8 iv[0x10];
    u8 iv_pad[0x10];
};

struct MetadataHeader
{
    be_t<u64> signature_input_length;
    be_t<u32> unknown1;
    be_t<u32> section_count;
    be_t<u32> key_count;
    be_t<u32> opt_header_size;
    be_t<u32> unknown2;
    be_t<u32> unknown3;
};

struct MetadataSectionHeader
{
    be_t<u64> data_offset;
    be_t<u64> data_size;
    be_t<u32> type;
    be_t<u32> program_idx;
    be_t<u32> hashed;
    be_t<u32> sha1_idx;
    be_t<u32> encrypted;
    be_t<u32> key_idx;
    be_t<u32> iv_idx;
    be_t<u32> compressed;
};

struct ControlInfo
{
    be_t<u32> type;
    be_t<u32> size;
    be_t<u64> next;

    union {
        struct {  // Type 1 (0x30 bytes)
            be_t<u32> ctrl_flag1;
            be_t<u32> unknown1;
            be_t<u32> unknown2;
            be_t<u32> unknown3;
            be_t<u32> unknown4;
            be_t<u32> unknown5;
            be_t<u32> unknown6;
            be_t<u32> unknown7;
        } control_flags;

        struct {  // Type 2 (0x30 bytes)
            u8 digest[20];
            u64 unknown;
        } file_digest40;

        struct {  // Type 2 (0x40 bytes)
          u8 digest1[20];
          u8 digest2[20];
          be_t<u64> unknown;
        } file_digest30;

        struct {  // Type 3 (0x90 bytes)
            be_t<u32> magic;
            be_t<u32> unknown1;
            be_t<u32> license;
            be_t<u32> type;
            u8 content_id[48];
            u8 digest[16];
            u8 invdigest[16];
            u8 xordigest[16];
            be_t<u64> unknown2;
            be_t<u64> unknown3;
        } npdrm;
    };
};

// ELF64 structs
struct Elf64_Ehdr
{
    be_t<u32> magic;
    be_t<u8>  elf_class;
    be_t<u8>  data;
    be_t<u8>  curver;
    be_t<u8>  os_abi;
    be_t<u64> abi_ver;
    be_t<u16> type;
    be_t<u16> machine;
    be_t<u32> version;
    be_t<u64> entry;
    be_t<u64> phoff;
    be_t<u64> shoff;
    be_t<u32> flags;
    be_t<u16> ehsize;
    be_t<u16> phentsize;
    be_t<u16> phnum;
    be_t<u16> shentsize;
    be_t<u16> shnum;
    be_t<u16> shstrndx;
};

struct Elf64_Phdr
{
    be_t<u32> type;
    be_t<u32> flags;
    be_t<u64> offset;
    be_t<u64> vaddr;
    be_t<u64> paddr;
    be_t<u64> filesz;
    be_t<u64> memsz;
    be_t<u64> align;
};

struct Elf64_Shdr
{
    be_t<u32> name;
    be_t<u32> type;
    be_t<u64> flags;
    be_t<u64> addr;
    be_t<u64> offset;
    be_t<u64> size;
    be_t<u32> link;
    be_t<u32> info;
    be_t<u64> addralign;
    be_t<u64> entsize;
};

enum ELFMachine
{
    MACHINE_UNKNOWN,
    MACHINE_MIPS = 0x08,
    MACHINE_PPC64 = 0x15,
    MACHINE_SPU = 0x17,
    MACHINE_ARM = 0x28,
};

class SELFLoader
{
    char* m_elf;
    char* m_self;
    size_t m_elf_size;
    size_t m_self_size;

    // Decrypts the Metadata Info and Headers of a SELF file
    bool decryptMetadata();

    // Returns the size of the ELF file after the SELF decryption/decompression
    // by using accessing the SELF's EHDR and decrypted Metadata headers.
    u32 getDecryptedElfSize();

public:
    SELFLoader();
    SELFLoader(const std::string& path);
    ~SELFLoader();

    bool open(const std::string& path);
    bool load_elf();
    bool load_prx(sys_prx_t* prx);
    bool decrypt();
    void close();

    u16 getMachine();
    u64 getEntry();
};
