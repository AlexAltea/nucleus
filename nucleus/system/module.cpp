/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"

#include "modules/libsysmodule.h"
#include "modules/libsysutil_avconf_ext.h"

namespace sys {

ModuleManager::ModuleManager()
{
    m_modules.emplace_back(Module("cellSysutil", {
        {0x0BAE8772, wrap(cellVideoOutConfigure)},
        {0x1E930EEF, wrap(cellVideoOutGetDeviceInfo)},
        {0xE558748D, wrap(cellVideoOutGetResolution)},
        {0x887572D5, wrap(cellVideoOutGetState)}
    }));
    m_modules.emplace_back(Module("cellSysutilAvconfExt", {
        {0x655A0364, wrap(cellVideoOutGetGamma)},
    }));
}

bool ModuleManager::find(const std::string& libraryName, U32 functionId)
{
    for (const auto& module : m_modules) {
        if (module.name != libraryName) {
            continue;
        }
        for (const auto& function : module.functions) {
            if (function.first != functionId) {
                continue;
            }
            return true;
        }
    }
    return false;
}

void ModuleManager::call(cpu::frontend::ppu::State& state)
{
    const U32 fnid = state.r[11];

    for (const auto& module : m_modules) {
        for (const auto& function : module.functions) {
            if (function.first == fnid) {
                function.second->call(state, nucleus.memory.getBaseAddr());
                return;
            }
        }
    }
    logger.warning(LOG_HLE, "Unknown Function ID: 0x%X", fnid);
}

}  // namespace sys
