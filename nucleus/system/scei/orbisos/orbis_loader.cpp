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
        logger.error(LOG_COMMON, "sce::scei::orbis: PKG loader unimplemented");
        return false;
    }
    else if (type == FILETYPE_SELF) {
        logger.error(LOG_COMMON, "sce::scei::orbis: SELF loader unimplemented");
        return false;
    }
    else if (type == FILETYPE_ELF) {
        Elf64_Ehdr<LE> eh;
        file->seek(0, fs::SeekSet);
        file->read(&eh, sizeof(eh));
        return eh.elf_class == ET_EXEC &&
            eh.abi_ver == ELFOSABI_FREEBSD && 
            eh.machine == EM_X86_64 &&
            eh.type == ET_SCE_EXEC;
    }
    return false;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
