/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_prx.h"
#include "nucleus/emulator.h"
#include "nucleus/system/callback.h"
#include "nucleus/system/lv2.h"
#include "nucleus/system/lv2/sys_process.h"
#include "nucleus/loader/self.h"

namespace sys {

S32 sys_prx_load_module(const S8* path, U64 flags, sys_prx_load_module_option_t* pOpt)
{
    SELFLoader self;
    auto file = nucleus.lv2.vfs.openFile(path, fs::Read);
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

    const S32 id = nucleus.lv2.objects.add(prx, SYS_PRX_OBJECT);
    return id;
}

S32 sys_prx_load_module_list(S32 count, BE<U64>* pathList, U64 flags, void* pOpt, BE<U32>* idList)
{
    for (S32 i = 0; i < count; i++) {
        auto* path = (S8*)((U64)nucleus.memory.getBaseAddr() + pathList[i]);
        auto* pOpt = nucleus.memory.ptr<sys_prx_load_module_option_t>(0);

        const S32 ret = sys_prx_load_module(path, flags, pOpt);
        if (ret <= 0) {
            return ret;
        }

        idList[i] = ret;
    }

    return CELL_OK;
}

S32 sys_prx_start_module(S32 id, U64 flags, sys_prx_start_module_option_t* pOpt)
{
    auto* prx = nucleus.lv2.objects.get<sys_prx_t>(id);
    const auto& param = nucleus.lv2.proc.prx_param;

    // Update ELF import table
    U32 offset = param.libstubstart;
    while (offset < param.libstubend) {
        const auto& importedLibrary = nucleus.memory.ref<sys_prx_library_info_t>(offset);
        offset += importedLibrary.size;

        for (const auto& lib : prx->exported_libs) {
            if (lib.name != nucleus.memory.ptr<S8>(importedLibrary.name_addr)) {
                continue;
            }
            for (U32 i = 0; i < importedLibrary.num_func; i++) {
                const U32 fnid = nucleus.memory.read32(importedLibrary.fnid_addr + 4*i);

                // Try to link to a native implementation (HLE)
                if (nucleus.lv2.modules.find(lib.name, fnid)) {
                    U32 hookAddr = nucleus.memory.alloc(20, 8);
                    nucleus.memory.write32(hookAddr + 0, 0x3D600000 | ((fnid >> 16) & 0xFFFF));  // lis  r11, fnid:hi
                    nucleus.memory.write32(hookAddr + 4, 0x616B0000 | (fnid & 0xFFFF));          // ori  r11, r11, fnid:lo
                    nucleus.memory.write32(hookAddr + 8, 0x44000042);                            // sc   2
                    nucleus.memory.write32(hookAddr + 12, 0x4E800020);                           // blr
                    nucleus.memory.write32(hookAddr + 16, hookAddr);                             // OPD: Function address
                    nucleus.memory.write32(hookAddr + 20, 0);                                    // OPD: Function RTOC
                    nucleus.memory.write32(importedLibrary.fstub_addr + 4*i, hookAddr + 16);
                }

                // Otherwise, link to original function (LLE)
                else {
                    nucleus.memory.write32(importedLibrary.fstub_addr + 4*i, lib.exports.at(fnid));
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

S32 sys_prx_0x1CE()
{
    return CELL_OK;
}

S32 sys_prx_get_module_list()
{
    return CELL_OK;
}

S32 sys_prx_register_library(U32 lib_addr)
{
    return CELL_OK;
}

S32 sys_prx_register_module()
{
    return CELL_OK;
}

}  // namespace sys
