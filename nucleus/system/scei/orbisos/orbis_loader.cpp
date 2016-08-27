/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbis_loader.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/orbisos/orbis_self.h"
#include "nucleus/system/elf64.h"
#include "nucleus/system/loader.h"

namespace sys {
namespace scei {
namespace orbis {

bool isValid(fs::File* file) {
    Filetype type = detectFiletype(file);

    if (type == FILETYPE_PKG) {
        logger.warning(LOG_COMMON, "sce::scei::orbis: PKG loader unimplemented");
        return false;
    }
    else if (type == FILETYPE_SELF) {
        logger.warning(LOG_COMMON, "sce::scei::orbis: SELF loader unimplemented");
        return false;
    }
    else if (type == FILETYPE_ELF) {
        Elf64_Ehdr<LE> eh;
        file->seek(0, fs::SeekSet);
        file->read(&eh, sizeof(eh));
        return (eh.elf_class == ELFCLASS64) &&
            (eh.abi_ver == ELFOSABI_FREEBSD || eh.abi_ver == ELFOSABI_NONE) &&
            (eh.type == ET_SCE_EXEC || eh.type == ET_SCE_UNKFE10) &&
            (eh.data == ELFDATA2LSB) &&
            (eh.machine == EM_X86_64);
    }
    return false;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
