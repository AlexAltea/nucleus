/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

enum ELFClass : U08 {
    ELFCLASSNONE = 0,  // Invalid class
    ELFCLASS32   = 1,  // 32-bit objects
    ELFCLASS64   = 2,  // 64-bit objects
};

enum ELFData : U08 {
    ELFDATANONE  = 0,  // Invalid data encoding
    ELFDATA2LSB  = 1,  // Least-significant byte first (little-endian)
    ELFDATA2MSB  = 2,  // Most-significant byte first (big-endian)
};

enum ELFABI : U08 {
    ELFOSABI_NONE        = 0,   // UNIX System V ABI
    ELFOSABI_HPUX        = 1,   // HP-UX operating system
    ELFOSABI_NETBSD      = 2,   // NetBSD
    ELFOSABI_LINUX       = 3,   // GNU/Linux
    ELFOSABI_HURD        = 4,   // GNU/Hurd
    ELFOSABI_SOLARIS     = 6,   // Solaris
    ELFOSABI_AIX         = 7,   // AIX
    ELFOSABI_IRIX        = 8,   // IRIX
    ELFOSABI_FREEBSD     = 9,   // FreeBSD
    ELFOSABI_TRU64       = 10,  // TRU64 UNIX
    ELFOSABI_MODESTO     = 11,  // Novell Modesto
    ELFOSABI_OPENBSD     = 12,  // OpenBSD
    ELFOSABI_STANDALONE  = 255, // Standalone application
};

enum ELFType : U16 {
    ET_NONE   = 0,       // No file type
    ET_REL    = 1,       // Relocatable object file
    ET_EXEC   = 2,       // Executable file
    ET_DYN    = 3,       // Shared object file
    ET_CORE   = 4,       // Core file
    ET_LOPROC = 0xFF00,  // Processor specific (start)
    ET_HIPROC = 0xFFFF,  // Processor specific (end)
};

enum ELFMachine {
    EM_NONE     = 0,    // No machine
    EM_SPARC    = 2,    // SPARC
    EM_PPC      = 20,   // PowerPC
    EM_PPC64    = 21,   // PowerPC64
    EM_SPU      = 23,   // IBM SPU/SPE
    EM_ARM      = 40,   // ARM
    EM_ALPHA    = 41,   // DEC Alpha
    EM_SPARCV9  = 43,   // SPARC V9
    EM_IA_64    = 50,   // Intel IA-64 processor architecture
    EM_X86_64   = 62,   // AMD x86-64 architecture
    EM_AARCH64  = 183,  // ARM AArch64
    EM_CUDA     = 190,  // NVIDIA CUDA architecture
    EM_AMDGPU   = 224,  // AMD GPU architecture
};

// Segment type
enum {
    PT_NULL    = 0,           // Unused segment.
    PT_LOAD    = 0x1,         // Loadable segment.
    PT_DYNAMIC = 0x2,         // Loadable segment.
    PT_INTERP  = 0x3,         // Interpreter pathname.
    PT_NOTE    = 0x4,         // Auxiliary information.
    PT_SHLIB   = 0x5,         // Reserved.
    PT_PHDR    = 0x6,         // The program header table itself.
    PT_TLS     = 0x7,         // The thread-local storage template.
    PT_LOOS    = 0x60000000,  // Lowest OS-specific entry type.
    PT_HIOS    = 0x6FFFFFFF,  // Highest OS-specific entry type.
    PT_LOPROC  = 0x70000000,  // Lowest CPU-specific entry type.
    PT_HIPROC  = 0x7FFFFFFF,  // Highest CPU-specific entry type.
};

// Segment flags
enum {
    PF_X       = 0x00000001,  // Segment is executable
    PF_W       = 0x00000002,  // Segment is writable
    PF_R       = 0x00000004,  // Segment is readable
};

/**
 * ELF header
 * ==========
 * @tparam  E  Endianness
 */
template <template <typename> typename E>
struct Elf64_Ehdr {
    E<U32> magic;
    E<U08> elf_class;
    E<U08> data;
    E<U08> curver;
    E<U08> os_abi;
    E<U64> abi_ver;
    E<U16> type;       // File type.
    E<U16> machine;    // Machine architecture.
    E<U32> version;    // ELF format version.
    E<U64> entry;      // Entry point.
    E<U64> phoff;      // Program header file offset.
    E<U64> shoff;      // Section header file offset.
    E<U32> flags;      // Architecture-specific flags.
    E<U16> ehsize;     // Size of ELF header in bytes.
    E<U16> phentsize;  // Size of program header entry.
    E<U16> phnum;      // Number of program header entries.
    E<U16> shentsize;  // Size of section header entry.
    E<U16> shnum;      // Number of section header entries.
    E<U16> shstrndx;   // Section name strings section.
};

/**
 * Program header
 * ==============
 * Table describing zero or more segments.
 * @tparam  E  Endianness
 */
template <template <typename> typename E>
struct Elf64_Phdr {
    E<U32> type;       // Entry type.
    E<U32> flags;      // Access permission flags.
    E<U64> offset;     // File offset of contents.
    E<U64> vaddr;      // Virtual address in memory image.
    E<U64> paddr;      // Physical address (not used).
    E<U64> filesz;     // Size of contents in file.
    E<U64> memsz;      // Size of contents in memory.
    E<U64> align;      // Alignment in memory and file.
};

/**
 * Section header
 * ==============
 * Table describing zero or more sections.
 * @tparam  E  Endianness
 */
template <template <typename> typename E>
struct Elf64_Shdr {
    E<U32> name;       // Section name (index into the section header string table).
    E<U32> type;       // Section type.
    E<U64> flags;      // Section flags.
    E<U64> addr;       // Address in memory image.
    E<U64> offset;     // Offset in file.
    E<U64> size;       // Size in bytes.
    E<U32> link;       // Index of a related section.
    E<U32> info;       // Depends on section type.
    E<U64> addralign;  // Alignment in bytes.
    E<U64> entsize;    // Size of each entry in section.
};

}  // namespace sys
