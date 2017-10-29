/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cellos_loader.h"
#include "nucleus/filesystem/filesystem_host.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/elf64.h"
#include "nucleus/system/loader.h"
#include "cellos_loader_self.h"

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
        SceHeader sceh;
        file->seek(0, fs::SeekSet);
        file->read(&sceh, sizeof(sceh));
        return (sceh.flags == 0x8000 || sceh.flags <= 0x1E) && sceh.type == 1;
    }
    else if (type == FILETYPE_ELF) {
        Elf64_Ehdr<BE> eh;
        file->seek(0, fs::SeekSet);
        file->read(&eh, sizeof(eh));
        return eh.elf_class == ELFCLASS64
            && eh.data == ELFDATA2MSB
            && eh.machine == EM_PPC64
            && eh.type == ET_EXEC;
    }
    return false;
}

}  // namespace cellos
}  // namespace scei
}  // namespace sys
