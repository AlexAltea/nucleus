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

// Segment types
enum {
    PT_SCE_DYNLIBDATA,  // (Unknown value. Unknown description.)
    PT_SCE_COMMENT,     // (Unknown value. Unknown description.)
    PT_SCE_LIBVERSION,  // (Unknown value. Unknown description.)
};

class SELFLoader {
    using Ehdr = sys::Elf64_Ehdr<LE>;
    using Phdr = sys::Elf64_Phdr<LE>;
    using Shdr = sys::Elf64_Shdr<LE>;

    std::vector<Byte> elf;  // Holds the decrypted executable
    std::vector<Byte> self; // Holds the encrypted executable

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

}  // namespace orbis
}  // namespace scei
}  // namespace sys
