/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

// Return codes
enum {
    CELL_LV2DBG_ERROR_DEINVALIDARGUMENTS = 0x80010409,  // Invalid arguments
    CELL_LV2DBG_ERROR_DEOPERATIONDENIED  = 0x8001042C,  // Operation denied
};

// Classes
union sys_dbg_spu_fpscr_t {
    U8  vrb[16];  // Byte
    U16 vrh[8];   // Half-word
    U32 vrw[4];   // Word
    U64 vrd[2];   // Double-word
};

union sys_dbg_spu_gpr_t {
    U8  vrb[16];  // Byte
    U16 vrh[8];   // Half-word
    U32 vrw[4];   // Word
    U64 vrd[2];   // Double-word
};

struct sys_dbg_spu_thread_context2_t {
    sys_dbg_spu_gpr_t gpr[128];  // General Purpose Registers GPR[0-127]
    U32 npc;                     // Next Program Counter
    sys_dbg_spu_fpscr_t fpscr;   // FPSCR
    U32 srr0;                    // SRR0
    U32 spu_status;              // SPU STATUS
    U64 spu_cfg;                 // SPU CFG
    U32 mb_stat;                 // MB_stat
    U32 ppu_mb;                  // PPU Mail Box
    U32 spu_mb[4];               // SPU Mail Box
    U32 decrementer;             // Decrementer
    U64 mfc_cq_sr[96];           // MFC Command Queue Save / Restore Register
};

// SysCalls
S32 sys_dbg_continue_processes();
S32 sys_dbg_continue_threads();
S32 sys_dbg_create_kernel_event_queue();
S32 sys_dbg_create_scratch_data_area_ext();
S32 sys_dbg_destroy_kernel_event_queue();
S32 sys_dbg_disable_floating_point_enabled_exception();
S32 sys_dbg_enable_floating_point_enabled_exception();
S32 sys_dbg_finalize_ppu_exception_handler();
S32 sys_dbg_get_address_from_dabr();
S32 sys_dbg_get_cond_list();
S32 sys_dbg_get_cond_information();
S32 sys_dbg_get_event_flag_list();
S32 sys_dbg_get_event_flag_information();
S32 sys_dbg_get_event_queue_list();
S32 sys_dbg_get_event_queue_information();
S32 sys_dbg_get_kernel_thread_list();
S32 sys_dbg_get_kernel_thread_info();
S32 sys_dbg_get_lwmutex_list();
S32 sys_dbg_get_lwmutex_information();
S32 sys_dbg_get_lwcond_list();
S32 sys_dbg_get_lwcond_information();
S32 sys_dbg_get_mutex_list();
S32 sys_dbg_get_mutex_information();
S32 sys_dbg_get_process_event_ctrl_flag();
S32 sys_dbg_get_raw_spu_list();
S32 sys_dbg_get_rwlock_list();
S32 sys_dbg_get_rwlock_information();
S32 sys_dbg_get_semaphore_list();
S32 sys_dbg_get_semaphore_information();
S32 sys_dbg_get_spu_thread_group_event_cntl_flag();
S32 sys_dbg_get_thread_list(U32 pid, u64* ids, u64* ids_num, u64* all_ids_num);
S32 sys_dbg_get_ppu_thread_name();
S32 sys_dbg_get_process_list();
S32 sys_dbg_get_process_info();
S32 sys_dbg_initialize_ppu_exception_handler();
S32 sys_dbg_kill_process();
S32 sys_dbg_ppu_exception_handler();
S32 sys_dbg_read_kernel_event_queue();
S32 sys_dbg_read_process_memory();
S32 sys_dbg_read_thread_register();
S32 sys_dbg_read_spu_thread_context2();
S32 sys_dbg_set_address_to_dabr();
S32 sys_dbg_set_process_event_cntl_flag();
S32 sys_dbg_set_run_control_bit_to_spu();
S32 sys_dbg_set_spu_thread_group_event_cntl_flag();
S32 sys_dbg_spu_thread_get_exception_cause();
S32 sys_dbg_spu_thread_read_from_ls();
S32 sys_dbg_spu_thread_write_to_ls();
S32 sys_dbg_stop_processes();
S32 sys_dbg_stop_threads();
S32 sys_dbg_vm_get_page_information();
S32 sys_dbg_vm_get_info();
S32 sys_dbg_write_process_memory();
S32 sys_dbg_write_thread_register();

}  // namespace sys
