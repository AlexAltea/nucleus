/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cellos_loader.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/self.h"
#include "nucleus/system/elf64.h"
#include "nucleus/system/loader.h"

namespace sys {
namespace scei {
namespace cellos {

bool isValid(fs::File* file) {
    Filetype type = detectFiletype(file);

    if (type == FILETYPE_PKG) {
        logger.warning(LOG_COMMON, "sce::scei::cellos: PKG loader unimplemented");
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
        return eh.elf_class == ELFCLASS64 &&
            eh.data == ELFDATA2MSB &&
            eh.abi_ver == ELFOSABI_CELL_LV2 && 
            eh.machine == EM_PPC64 &&
            eh.type == ET_EXEC;
    }
    return false;
}

}  // namespace cellos
}  // namespace scei
}  // namespace sys
