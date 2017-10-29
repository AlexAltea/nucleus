/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/cellos/lv2.h"

#include "modules/libsysutil.h"
#include "modules/libsysutil_avconf_ext.h"

namespace sys {

ModuleManager::ModuleManager(LV2* parent) : parent(parent) {
    modules.emplace_back(Module("cellSysutil", {
        {0x0BAE8772, wrap(cellVideoOutConfigure)},
        {0x1E930EEF, wrap(cellVideoOutGetDeviceInfo)},
        {0xE558748D, wrap(cellVideoOutGetResolution)},
        {0x887572D5, wrap(cellVideoOutGetState)}
    }));
    modules.emplace_back(Module("cellSysutilAvconfExt", {
        {0x655A0364, wrap(cellVideoOutGetGamma)},
    }));
}

bool ModuleManager::find(const std::string& libraryName, U32 functionId) {
    for (const auto& module : modules) {
        if (module.name != libraryName) {
            continue;
        }
        for (const auto& function : module.functions) {
            if (function.first == functionId) {
                return true;
            }
        }
    }
    return false;
}

void ModuleManager::call(cpu::frontend::ppu::PPUState& state, LV2* kernel) {
    const U32 fnid = static_cast<U32>(state.r[11]);
    call(state, kernel, fnid);
}

void ModuleManager::call(cpu::frontend::ppu::PPUState& state, LV2* kernel, U32 fnid) {
    for (const auto& module : modules) {
        for (const auto& function : module.functions) {
            if (function.first == fnid) {
                function.second->call(state, *kernel, parent->memory->getBaseAddr());
                return;
            }
        }
    }
    logger.warning(LOG_HLE, "Unknown Function ID: 0x%X", fnid);
}

}  // namespace sys
