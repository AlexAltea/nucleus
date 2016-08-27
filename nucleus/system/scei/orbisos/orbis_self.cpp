/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbis_self.h"
#include "nucleus/assert.h"
#include "nucleus/system/loader.h"

#include <algorithm>
#include <cstring>

namespace sys {
namespace scei {
namespace orbis {

bool SELFLoader::open(fs::File* file) {
    const Size selfSize = file->attributes().size;
    self.resize(selfSize);
    file->seek(0, fs::SeekSet);
    file->read(&self[0], selfSize);

    switch (detectFiletype(file)) {
    case FILETYPE_ELF:
        elf.resize(selfSize);
        memcpy(&elf[0], &self[0], selfSize);
        return true;

    default:
        return false;
    }
}

void* SELFLoader::load() {
    assert_true(!elf.empty(), "No ELF/SELF file has been opened");
    const auto& eh = (Ehdr&)elf[0];

    // Get ELF size
    Size low = ~0ULL;
    Size high = 0ULL;
    for (Size i = 0; i < eh.phnum; i++) {
        const auto& ph = (Phdr&)elf[eh.phoff + i*sizeof(Phdr)];
        low = std::min(low, Size(ph.vaddr));
        high = std::max(high, Size(ph.vaddr + ph.memsz));
    }
    Size size = high - low;
    void* elf_addr = malloc(size);

    // Loading program header table
    for (U64 i = 0; i < eh.phnum; i++) {
        const auto& ph = (Phdr&)elf[eh.phoff + i*sizeof(Phdr)];

        switch (ph.type) {
        case PT_LOAD: {
            if (!ph.memsz) {
                break;
            }
            void* vaddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(elf_addr) + ph.vaddr);
            memcpy(vaddr, &elf[ph.offset], ph.filesz);
            if (ph.flags & PF_X) {

            }
            break;
        }

        case PT_DYNAMIC:
        case PT_INTERP:
        case PT_TLS:
        case PT_SCE_DYNLIBDATA:
        case PT_SCE_PROCPARAM:
        case PT_SCE_UNK61000010:
        case PT_GNU_EH_FRAME:
        case PT_SCE_COMMENT:
        case PT_SCE_LIBVERSION:
            // TODO: ?
            break;

        default:
            assert_always("Unexpected segment type");
        }
    }
    return elf_addr;
}

U16 SELFLoader::getMachine() {
    if (elf.empty()) {
        return EM_NONE;
    }
    const auto& ehdr = (Ehdr&)elf[0];
    return ehdr.machine;
}

U64 SELFLoader::getEntry() {
    if (elf.empty()) {
        return 0;
    }
    const auto& ehdr = (Ehdr&)elf[0];
    return ehdr.entry;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
