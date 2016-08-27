/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbis_dynlib.h"
#include "nucleus/system/scei/orbisos/orbisos.h"

namespace sys {
namespace scei {
namespace orbis {

S64 sys_dynlib_load_prx(void* thread, dynlib_load_prx_args* args) {
    return SCE_OK;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
