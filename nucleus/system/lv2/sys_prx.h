/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace sys {

// Return codes
enum {
    CELL_PRX_ERROR_ERROR                       = 0x80011001,  // Error state
    CELL_PRX_ERROR_ILLEGAL_PERM                = 0x800110D1,  // No permission to execute API
    CELL_PRX_ERROR_UNKNOWN_MODULE              = 0x8001112E,  // Specified PRX could not be found
    CELL_PRX_ERROR_ALREADY_STARTED             = 0x80011133,  // Specified PRX is already started
    CELL_PRX_ERROR_NOT_STARTED                 = 0x80011134,  // Specified PRX is not started
    CELL_PRX_ERROR_ALREADY_STOPPED             = 0x80011135,  // Specified PRX is already stopped
    CELL_PRX_ERROR_CAN_NOT_STOP                = 0x80011136,  // Specified PRX must not be stopped
    CELL_PRX_ERROR_NOT_REMOVABLE               = 0x80011138,  // Specified PRX must not be deleted
    CELL_PRX_ERROR_LIBRARY_NOT_YET_LINKED      = 0x8001113A,  // Called unlinked function
    CELL_PRX_ERROR_LIBRARY_FOUND               = 0x8001113B,  // Specified library is already registered
    CELL_PRX_ERROR_LIBRARY_NOTFOUND            = 0x8001113C,  // Specified library is not registered
    CELL_PRX_ERROR_ILLEGAL_LIBRARY             = 0x8001113D,  // Library structure is invalid
    CELL_PRX_ERROR_LIBRARY_INUSE               = 0x8001113E,  // Library cannot be deleted because it is linked
    CELL_PRX_ERROR_ALREADY_STOPPING            = 0x8001113F,  // Specified PRX is in the process of stopping
    CELL_PRX_ERROR_UNSUPPORTED_PRX_TYPE        = 0x80011148,  // Specified PRX format is invalid and cannot be loaded
    CELL_PRX_ERROR_INVAL                       = 0x80011324,  // Argument value is invalid
    CELL_PRX_ERROR_ILLEGAL_PROCESS             = 0x80011801,  // Specified process does not exist
    CELL_PRX_ERROR_NO_LIBLV2                   = 0x80011881,  // liblv2.sprx does not exist
    CELL_PRX_ERROR_UNSUPPORTED_ELF_TYPE        = 0x80011901,  // ELF type of specified file is not supported
    CELL_PRX_ERROR_UNSUPPORTED_ELF_CLASS       = 0x80011902,  // ELF class of specified file is not supported
    CELL_PRX_ERROR_UNDEFINED_SYMBOL            = 0x80011904,  // References undefined symbols
    CELL_PRX_ERROR_UNSUPPORTED_RELOCATION_TYPE = 0x80011905,  // Uses unsupported relocation type
    CELL_PRX_ERROR_ELF_IS_REGISTERED           = 0x80011910,  // Fixed ELF is already registered
};

// PRX file headers
struct sys_prx_module_info_t
{
    BE<U16> attributes;
    BE<U16> version;
    S8 name[28];
    BE<U32> toc;
    BE<U32> exports_start;
    BE<U32> exports_end;
    BE<U32> imports_start;
    BE<U32> imports_end;
};

// Information about imported or exported libraries in PRX modules
struct sys_prx_library_info_t
{
    U8 size;
    U8 unk0;
    BE<U16> version;
    BE<U16> attributes;
    BE<U16> num_func;
    BE<U16> num_var;
    BE<U16> num_tlsvar;
    U8 info_hash;
    U8 info_tlshash;
    U8 unk1[2];
    BE<U32> name_addr;
    BE<U32> fnid_addr;
    BE<U32> fstub_addr;
    BE<U32> unk4;
    BE<U32> unk5;
    BE<U32> unk6;
    BE<U32> unk7;
};

// Relocation information of the SCE_PPURELA segment
struct sys_prx_relocation_info_t
{
    BE<U64> offset;
    BE<U16> unk0;
    U8 index_value;    // Index of the LOAD segment that updated its base address
    U8 index_addr;     // Index of the LOAD segment containing the address to patch
    BE<U32> type;
    BE<U64> ptr;
};


// Data types
struct sys_prx_load_module_option_t
{
    BE<U64> size;
    BE<U32> base_addr;
};

struct sys_prx_start_module_option_t
{
    BE<U64> size;
    BE<U64> unk0;
    BE<U64> entry;
};

struct sys_prx_stop_module_option_t
{
    BE<U64> size;
};

struct sys_prx_unload_module_option_t
{
    BE<U64> size;
};

// Auxiliary classes
struct sys_prx_library_t
{
    std::string name;
    std::unordered_map<U32, U32> exports; // Map: FNID (32-bit) -> Export table offset (32-bit)
};

struct sys_prx_segment_t
{
    U32 addr;          // Base address where the PRX segment has been copied
    U32 flags;         // R|W|X-flags for the PPU/SPU/RSX (PHDR's flags)
    U32 align;         // Alignment (PHDR's align)
    U32 size_file;     // Size of the segment in the ELF file (PHDR's filesz)
    U32 size_memory;   // Size of the segment in memory (PHDR's memsz). Filled with zeros after allocation
    U32 initial_addr;  // Base address specified on PHDR header. Used to update the import table
    U32 prx_offset;    // Base offset in the PRX file
};

// Auxiliary classes
struct sys_prx_t
{
    U16 version;
    U32 func_start;
    U32 func_stop;
    U32 func_exit;
    std::string name;  // Name of the module
    std::string path;  // Path to the PRX/SPRX file
    std::vector<sys_prx_library_t> exported_libs;
    std::vector<sys_prx_library_t> imported_libs;
    std::vector<sys_prx_segment_t> segments;
};

// SysCalls
S32 sys_prx_0x1CE();
S32 sys_prx_load_module(const S8* path, U64 flags, sys_prx_load_module_option_t* pOpt);
S32 sys_prx_load_module_by_fd();
S32 sys_prx_load_module_list(S32 count, BE<U64>* pathList, U64 flags, void* pOpt, BE<U32>* idList);
S32 sys_prx_load_module_on_memcontainer();
S32 sys_prx_load_module_on_memcontainer_by_fd();
S32 sys_prx_start_module(S32 id, U64 flags, sys_prx_start_module_option_t* pOpt);
S32 sys_prx_stop_module(S32 id, U32 args, U32 argp_addr, BE<U32>* modres, U64 flags, sys_prx_stop_module_option_t* pOpt);
S32 sys_prx_unload_module(S32 id, U64 flags, sys_prx_unload_module_option_t* pOpt);
S32 sys_prx_register_module();
S32 sys_prx_get_module_list();
S32 sys_prx_get_my_module_id();
S32 sys_prx_get_module_id_by_address();
S32 sys_prx_get_module_id_by_name();
S32 sys_prx_get_module_info();
S32 sys_prx_register_library(U32 lib_addr);
S32 sys_prx_unregister_library();
S32 sys_prx_get_ppu_guid();
S32 sys_prx_query_module();
S32 sys_prx_link_library();
S32 sys_prx_unlink_library();
S32 sys_prx_query_library();
S32 sys_prx_start();
S32 sys_prx_stop();

}  // namespace sys
