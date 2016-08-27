/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/file.h"
#include "nucleus/system/elf64.h"

#include <string>
#include <vector>

namespace sys {
namespace scei {
namespace orbis {

// ELF types
enum {
    ET_SCE_EXEC    = 0xFE00,  // SCE Executable file
    ET_SCE_RELEXEC = 0xFE04,  // SCE Relocatable file
    ET_SCE_STUBLIB = 0xFE0C,  // SCE SDK Stubs
    ET_SCE_UNKFE10 = 0xFE10,  // Unknown (Probably PS4 Executable file with ASLR)
    ET_SCE_DYNAMIC = 0xFE18,
};

// Segment types
enum {
    PT_GNU_EH_FRAME    = 0x6474E550,
    PT_SCE_RELA        = 0x60000000,
    PT_SCE_DYNLIBDATA  = 0x61000000,
    PT_SCE_PROCPARAM   = 0x61000001,
    PT_SCE_UNK61000010 = 0x61000010,
    PT_SCE_COMMENT     = 0x6FFFFF00,
    PT_SCE_LIBVERSION  = 0x6FFFFF01,
    PT_SCE_PSPRELA     = 0x700000A0,
    PT_SCE_PPURELA     = 0x700000A4,
};

class SELFLoader {
    using Ehdr = sys::Elf64_Ehdr<LE>;
    using Phdr = sys::Elf64_Phdr<LE>;
    using Shdr = sys::Elf64_Shdr<LE>;

    std::vector<Byte> elf;  // Holds the decrypted executable
    std::vector<Byte> self; // Holds the encrypted executable

    /**
     * Returns the length of the interval that covers all LOAD segments
     * @return  Size in bytes
     */
    Size getRequiredSize();

public:
    bool open(fs::File* file);
    void* load();
    void close();

    U16 getMachine();
    U64 getEntry();
};

}  // namespace orbis
}  // namespace scei
}  // namespace sys
