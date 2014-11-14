/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Return codes
enum {
    CELL_LV2DBG_ERROR_DEINVALIDARGUMENTS = 0x80010409,  // Invalid arguments
    CELL_LV2DBG_ERROR_DEOPERATIONDENIED  = 0x8001042c,  // Operation denied
};

// Classes
union sys_dbg_spu_fpscr_t {
    u8  vrb[16];  // Byte
    u16 vrh[8];   // Half-word
    u32 vrw[4];   // Word
    u64 vrd[2];   // Double-word
};

union sys_dbg_spu_gpr_t {
    u8  vrb[16];  // Byte
    u16 vrh[8];   // Half-word
    u32 vrw[4];   // Word
    u64 vrd[2];   // Double-word
};

struct sys_dbg_spu_thread_context2_t {
    sys_dbg_spu_gpr_t gpr[128];  // General Purpose Registers GPR[0-127]
    u32 npc;                     // Next Program Counter
    sys_dbg_spu_fpscr_t fpscr;   // FPSCR
    u32 srr0;                    // SRR0
    u32 spu_status;              // SPU STATUS
    u64 spu_cfg;                 // SPU CFG
    u32 mb_stat;                 // MB_stat
    u32 ppu_mb;                  // PPU Mail Box
    u32 spu_mb[4];               // SPU Mail Box
    u32 decrementer;             // Decrementer
    u64 mfc_cq_sr[96];           // MFC Command Queue Save / Restore Register
};

// SysCalls
s32 sys_dbg_continue_processes();
s32 sys_dbg_continue_threads();
s32 sys_dbg_create_kernel_event_queue();
s32 sys_dbg_create_scratch_data_area_ext();
s32 sys_dbg_destroy_kernel_event_queue();
s32 sys_dbg_disable_floating_point_enabled_exception();
s32 sys_dbg_enable_floating_point_enabled_exception();
s32 sys_dbg_finalize_ppu_exception_handler();
s32 sys_dbg_get_address_from_dabr();
s32 sys_dbg_get_cond_list();
s32 sys_dbg_get_cond_information();
s32 sys_dbg_get_event_flag_list();
s32 sys_dbg_get_event_flag_information();
s32 sys_dbg_get_event_queue_list();
s32 sys_dbg_get_event_queue_information();
s32 sys_dbg_get_kernel_thread_list();
s32 sys_dbg_get_kernel_thread_info();
s32 sys_dbg_get_lwmutex_list();
s32 sys_dbg_get_lwmutex_information();
s32 sys_dbg_get_lwcond_list();
s32 sys_dbg_get_lwcond_information();
s32 sys_dbg_get_mutex_list();
s32 sys_dbg_get_mutex_information();
s32 sys_dbg_get_process_event_ctrl_flag();
s32 sys_dbg_get_raw_spu_list();
s32 sys_dbg_get_rwlock_list();
s32 sys_dbg_get_rwlock_information();
s32 sys_dbg_get_semaphore_list();
s32 sys_dbg_get_semaphore_information();
s32 sys_dbg_get_spu_thread_group_event_cntl_flag();
s32 sys_dbg_get_thread_list(u32 pid, u64* ids, u64* ids_num, u64* all_ids_num);
s32 sys_dbg_get_ppu_thread_name();
s32 sys_dbg_get_process_list();
s32 sys_dbg_get_process_info();
s32 sys_dbg_initialize_ppu_exception_handler();
s32 sys_dbg_kill_process();
s32 sys_dbg_ppu_exception_handler();
s32 sys_dbg_read_kernel_event_queue();
s32 sys_dbg_read_process_memory();
s32 sys_dbg_read_thread_register();
s32 sys_dbg_read_spu_thread_context2();
s32 sys_dbg_set_address_to_dabr();
s32 sys_dbg_set_process_event_cntl_flag();
s32 sys_dbg_set_run_control_bit_to_spu();
s32 sys_dbg_set_spu_thread_group_event_cntl_flag();
s32 sys_dbg_spu_thread_get_exception_cause();
s32 sys_dbg_spu_thread_read_from_ls();
s32 sys_dbg_spu_thread_write_to_ls();
s32 sys_dbg_stop_processes();
s32 sys_dbg_stop_threads();
s32 sys_dbg_vm_get_page_information();
s32 sys_dbg_vm_get_info();
s32 sys_dbg_write_process_memory();
s32 sys_dbg_write_thread_register();
