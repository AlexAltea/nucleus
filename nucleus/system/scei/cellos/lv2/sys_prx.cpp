/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_prx.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/core/config.h"
#include "nucleus/system/scei/cellos/callback.h"
#include "nucleus/system/scei/cellos/lv2/sys_process.h"
#include "nucleus/system/scei/self.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_prx_load_module, const S08* path, U64 flags, sys_prx_load_module_option_t* pOpt) {
    SELFLoader self;
    auto file = kernel.vfs.openFile(path, fs::Read);
    if (!self.open(file)) {
        return CELL_PRX_ERROR_UNKNOWN_MODULE;
    }
    delete file;

    // Load PRX into memory
    auto* prx = new sys_prx_t();
    if (!self.load_prx(*prx)) {
        delete prx;
        return CELL_PRX_ERROR_ILLEGAL_LIBRARY;
    }

    // Obtain module start/stop/exit functions (set to 0 if not present)
    auto& metaLib = prx->exported_libs[0];
    prx->func_start = metaLib.exports[0xBC9A0086];
    prx->func_stop = metaLib.exports[0xAB779874];
    prx->func_exit = metaLib.exports[0x3AB9A95E];
    prx->path = path;

    const S32 id = kernel.objects.add(prx, SYS_PRX_OBJECT);
    return id;
}

HLE_FUNCTION(sys_prx_load_module_list, S32 count, BE<U64>* pathList, U64 flags, void* pOpt, BE<U32>* idList) {
    for (S32 i = 0; i < count; i++) {
        auto* path = (S08*)((U64)kernel.memory->getBaseAddr() + pathList[i]);
        auto* pOpt = kernel.memory->ptr<sys_prx_load_module_option_t>(0);

        const S32 ret = sys_prx_load_module(kernel, path, flags, pOpt);
        if (ret <= 0) {
            return ret;
        }

        idList[i] = ret;
    }

    return CELL_OK;
}

HLE_FUNCTION(sys_prx_start_module, S32 id, U64 flags, sys_prx_start_module_option_t* pOpt) {
    auto* prx = kernel.objects.get<sys_prx_t>(id);
    const auto& param = kernel.proc.prx_param;

    auto* cpu = dynamic_cast<cpu::GuestCPU*>(kernel.getEmulator()->cpu.get());

    // Update ELF import table
    U32 offset = param.libstubstart;
    while (offset < param.libstubend) {
        const auto& importedLibrary = kernel.memory->ref<sys_prx_library_info_t>(offset);
        offset += importedLibrary.size;

        for (const auto& lib : prx->exported_libs) {
            if (lib.name != kernel.memory->ptr<S08>(importedLibrary.name_addr)) {
                continue;
            }
            for (U32 i = 0; i < importedLibrary.num_func; i++) {
                const U32 fnid = kernel.memory->read32(importedLibrary.fnid_addr + 4*i);

                // Try to link to a native implementation (HLE)
                if (kernel.modules.find(lib.name, fnid)) {
                    if (config.ppuTranslator == CPU_TRANSLATOR_INSTRUCTION) {
                        U32 hookAddr = kernel.memory->alloc(20, 8);
                        kernel.memory->write32(hookAddr + 0, 0x3D600000 | ((fnid >> 16) & 0xFFFF));  // lis  r11, fnid:hi
                        kernel.memory->write32(hookAddr + 4, 0x616B0000 | (fnid & 0xFFFF));          // ori  r11, r11, fnid:lo
                        kernel.memory->write32(hookAddr + 8, 0x44000042);                            // sc   2
                        kernel.memory->write32(hookAddr + 12, 0x4E800020);                           // blr
                        kernel.memory->write32(hookAddr + 16, hookAddr);                             // OPD: Function address
                        kernel.memory->write32(hookAddr + 20, 0);                                    // OPD: Function RTOC
                        kernel.memory->write32(importedLibrary.fstub_addr + 4*i, hookAddr + 16);
                    }
                    if (config.ppuTranslator == CPU_TRANSLATOR_FUNCTION) {
                        const U32 addr = lib.exports.at(fnid);
                        const U32 func_addr = kernel.memory->read32(addr + 0);
                        const U32 func_rtoc = kernel.memory->read32(addr + 4);
                        for (auto& module : cpu->ppu_modules) {
                            if (module->contains(func_addr)) {
                                module->hook(func_addr, fnid);
                                break;
                            }
                        }
                        kernel.memory->write32(importedLibrary.fstub_addr + 4*i, addr);
                    }
                }

                // Otherwise, link to original function (LLE)
                else {
                    kernel.memory->write32(importedLibrary.fstub_addr + 4*i, lib.exports.at(fnid));
                }
            }
        }
    }

    if (prx->func_start) {
        pOpt->entry = prx->func_start;
    } else {
        pOpt->entry = 0xFFFFFFFFFFFFFFFFULL;
    }

    return CELL_OK;
}

HLE_FUNCTION(sys_prx_0x1CE) {
    return CELL_OK;
}

HLE_FUNCTION(sys_prx_get_module_list) {
    return CELL_OK;
}

HLE_FUNCTION(sys_prx_get_module_id_by_name, const S08* name, U64 flags, sys_prx_get_module_id_by_name_option_t* pOpt) {
    if (name == kernel.memory->ptr(0)) {
        return 0;
    }
    // Find library (TODO: This is very inefficient)
    for (const auto& object : kernel.objects) {
        if (object.second->getType() == sys::SYS_PRX_OBJECT) {
            const auto& prx = *(sys_prx_t*)object.second->getData();
            if (prx.name == name) {
                return object.first;
            }
        }
    }
    return 0;
}

HLE_FUNCTION(sys_prx_register_library, U32 lib_addr) {
    return CELL_OK;
}

HLE_FUNCTION(sys_prx_register_module) {
    return CELL_OK;
}

}  // namespace sys
