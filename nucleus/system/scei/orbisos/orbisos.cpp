/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbisos.h"

namespace sys {
namespace scei {
namespace orbis {

OrbisOS::OrbisOS() {
    // Initialize sandbox directories
    vfs.registerDevice(new fs::HostPathDevice("/host/", ""));
}

bool OrbisOS::init(U64 entry) {
    // TODO: Load /system/common/lib/libkernel.sprx

    // TODO: Run libkernel.sprx start function

    // TODO: Jump to executable entry point

    return true;
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
