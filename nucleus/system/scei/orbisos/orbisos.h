/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/system/system.h"
#include "nucleus/system/scei/cellos/lv2/sys_process.h" // TODO: Just for sys_process_t

namespace sys {
namespace scei {
namespace orbis {

struct OrbisSyscall {
    const char* name;
};

class OrbisOS : public System {
public:
    // Kernel information
    sys_process_t proc; // TODO: Is this really necessary

    OrbisOS();

    /**
     * Start the process at the given entry point
     * @param[in]  entry  Module entry point
     */
    bool init(U64 entry);
};

}  // namespace orbis
}  // namespace scei
}  // namespace sys
