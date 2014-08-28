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

public:
    SELFLoader();
    SELFLoader(const std::string& path);
    ~SELFLoader();

    bool open(const std::string& path);
    bool load();
    bool decrypt();
    void close();

    bool isDebug();
    u16 getMachine();
    u64 getEntry();
};
