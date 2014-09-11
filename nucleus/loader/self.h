/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include <string>

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
        // Type 1 (0x30 bytes)
        struct {
            be_t<u32> ctrl_flag1;
            be_t<u32> unknown1;
            be_t<u32> unknown2;
            be_t<u32> unknown3;
            be_t<u32> unknown4;
            be_t<u32> unknown5;
            be_t<u32> unknown6;
            be_t<u32> unknown7;
       } control_flags;

        // Type 2 (0x30 bytes)
        struct {
            u8 digest[20];
            u64 unknown;
        } file_digest40;

        // Type 2 (0x40 bytes)
        struct {
          u8 digest1[20];
          u8 digest2[20];
          be_t<u64> unknown;
        } file_digest30;

        // Type 3 (0x90 bytes)
        struct {
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

    bool decryptMetadata();

public:
    SELFLoader();
    SELFLoader(const std::string& path);
    ~SELFLoader();

    bool open(const std::string& path);
    bool load();
    bool decrypt();
    void close();

    u16 getMachine();
    u64 getEntry();
};
