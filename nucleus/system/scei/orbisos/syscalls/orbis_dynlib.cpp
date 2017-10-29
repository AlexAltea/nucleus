/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbis_dynlib.h"
#include "nucleus/system/scei/orbisos/orbisos.h"
#include "nucleus/system/scei/orbisos/orbis_self.h"
#include "nucleus/cpu/host/x86/x86_proxy.h"
#include "nucleus/logger/logger.h"
#include "nucleus/filesystem/filesystem_host.h"

namespace sys {
namespace scei {
namespace orbis {

S64 sys_dynlib_load_prx(void* thread, dynlib_load_prx_args* args) {
    SELFLoader self;
    auto file = fs::HostFileSystem::openFile(args->path, fs::Read);
    if (!self.open(file.get())) {
        logger.error(LOG_COMMON, "Invalid file given.");
        return false;
    }
    void* entryBase = self.load();
    //cpu::native::x86::setStateAndEnter64();
    return SCE_OK;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
