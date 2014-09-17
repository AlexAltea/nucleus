/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_prx.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/memory/object.h"
#include "nucleus/loader/self.h"
sys_prx_t prx; //REMOVE ME
s32 sys_prx_load_module(vm_ptr<s8> path, u64 flags, vm_ptr<sys_prx_load_module_option_t> pOpt)
{
    SELFLoader self;
    if (!self.open(path.ptr())) {
        return CELL_PRX_ERROR_UNKNOWN_MODULE;
    }
    
    prx.path = path.ptr();
    if (!self.load_prx(prx)) {
        return CELL_PRX_ERROR_ILLEGAL_LIBRARY;
    }

    return CELL_OK;
}

s32 sys_prx_start_module(s32 id, u32 args, u32 argp_addr, vm_ptr<be_t<u32>> modres, u64 flags, vm_ptr<sys_prx_start_module_option_t> pOpt)
{
    const u32 elf_base = 0x10000;
    const auto& ehdr = (Elf64_Ehdr&)nucleus.memory[elf_base];

    for (u64 i = 0; i < ehdr.phnum; i++) {
        const auto& phdr = (Elf64_Phdr&)nucleus.memory[elf_base + ehdr.phoff + i*sizeof(Elf64_Phdr)];
        if (phdr.type != 0x60000002) {
            continue;
        }

        const auto& prx_param = (sys_prx_param_t&)nucleus.memory[phdr.vaddr];

        // Update import table
        u32 offset = prx_param.libstubstart;
        while (offset < prx_param.libstubend) {
            const auto& importedLibrary = (sys_prx_library_info_t&)nucleus.memory[offset];
            offset += importedLibrary.size;

            for (const auto& lib : prx.libraries) {
                if (lib.name != (s8*)(nucleus.memory + importedLibrary.name_addr)) {
                    continue;
                }
                for (u32 i = 0; i < importedLibrary.num_func; i++) {
                    const u32 fnid = nucleus.memory.read32(importedLibrary.fnid_addr + 4*i);
                    nucleus.memory.write32(importedLibrary.fstub_addr + 4*i, lib.exports.at(fnid));
                }
            }
        }
    }

    return CELL_OK;
}
