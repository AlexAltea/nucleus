/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv2.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/filesystem_app.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/cellos/callback.h"

#include "lv2/sys_cond.h"
#include "lv2/sys_config.h"
#include "lv2/sys_dbg.h"
#include "lv2/sys_event.h"
#include "lv2/sys_fs.h"
#include "lv2/sys_gamepad.h"
#include "lv2/sys_hid.h"
#include "lv2/sys_lwmutex.h"
#include "lv2/sys_memory.h"
#include "lv2/sys_mmapper.h"
#include "lv2/sys_mutex.h"
#include "lv2/sys_ppu_thread.h"
#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"
#include "lv2/sys_rsx.h"
#include "lv2/sys_semaphore.h"
#include "lv2/sys_ss.h"
#include "lv2/sys_spu.h"
#include "lv2/sys_time.h"
#include "lv2/sys_timer.h"
#include "lv2/sys_tty.h"

#include <cstring>

#define SYSCALL(name, flags) { wrap(name), #name, flags }

namespace sys {

LV2::LV2(std::shared_ptr<mem::Memory> memory, U32 fw_type) : memory(std::move(memory)), modules(this) {
    // Initialize syscall table
    memset(syscalls, 0, sizeof(syscalls));
    if (fw_type & (LV2_CEX | LV2_DEX | LV2_DECR)) {
        syscalls[0x001] = SYSCALL(sys_process_getpid, LV2_NONE);
        syscalls[0x003] = SYSCALL(sys_process_exit, LV2_NONE);
        syscalls[0x00E] = SYSCALL(sys_process_is_spu_lock_line_reservation_address, LV2_NONE);
        syscalls[0x012] = SYSCALL(sys_process_getppid, LV2_NONE);
        syscalls[0x016] = SYSCALL(sys_process_exit, LV2_NONE);
        syscalls[0x019] = SYSCALL(sys_process_get_sdk_version, LV2_NONE);
        syscalls[0x01E] = SYSCALL(sys_process_get_paramsfo, LV2_NONE);
        syscalls[0x029] = SYSCALL(sys_ppu_thread_exit, LV2_NONE);
        syscalls[0x02C] = SYSCALL(sys_ppu_thread_join, LV2_NONE);
        syscalls[0x030] = SYSCALL(sys_ppu_thread_get_priority, LV2_NONE);
        syscalls[0x031] = SYSCALL(sys_ppu_thread_get_stack_information, LV2_NONE);
        syscalls[0x034] = SYSCALL(sys_ppu_thread_create, LV2_NONE);
        syscalls[0x035] = SYSCALL(sys_ppu_thread_start, LV2_NONE);
        syscalls[0x052] = SYSCALL(sys_event_flag_create, LV2_NONE);
        syscalls[0x053] = SYSCALL(sys_event_flag_destroy, LV2_NONE);
        syscalls[0x055] = SYSCALL(sys_event_flag_wait, LV2_NONE);
        syscalls[0x056] = SYSCALL(sys_event_flag_trywait, LV2_NONE);
        syscalls[0x057] = SYSCALL(sys_event_flag_set, LV2_NONE);
        syscalls[0x05A] = SYSCALL(sys_semaphore_create, LV2_NONE);
        syscalls[0x05B] = SYSCALL(sys_semaphore_destroy, LV2_NONE);
        syscalls[0x05C] = SYSCALL(sys_semaphore_wait, LV2_NONE);
        syscalls[0x05D] = SYSCALL(sys_semaphore_trywait, LV2_NONE);
        syscalls[0x05E] = SYSCALL(sys_semaphore_post, LV2_NONE);
        syscalls[0x05F] = SYSCALL(sys_lwmutex_create, LV2_NONE);
        syscalls[0x060] = SYSCALL(sys_lwmutex_destroy, LV2_NONE);
        syscalls[0x061] = SYSCALL(sys_lwmutex_lock, LV2_NONE);
        syscalls[0x062] = SYSCALL(sys_lwmutex_trylock, LV2_NONE);
        syscalls[0x063] = SYSCALL(sys_lwmutex_unlock, LV2_NONE);
        syscalls[0x064] = SYSCALL(sys_mutex_create, LV2_NONE);
        syscalls[0x065] = SYSCALL(sys_mutex_destroy, LV2_NONE);
        syscalls[0x066] = SYSCALL(sys_mutex_lock, LV2_NONE);
        syscalls[0x067] = SYSCALL(sys_mutex_trylock, LV2_NONE);
        syscalls[0x068] = SYSCALL(sys_mutex_unlock, LV2_NONE);
        syscalls[0x069] = SYSCALL(sys_cond_create, LV2_NONE);
        syscalls[0x06A] = SYSCALL(sys_cond_destroy, LV2_NONE);
        syscalls[0x06B] = SYSCALL(sys_cond_wait, LV2_NONE);
        syscalls[0x06C] = SYSCALL(sys_cond_signal, LV2_NONE);
        syscalls[0x06D] = SYSCALL(sys_cond_signal_all, LV2_NONE);
        syscalls[0x06E] = SYSCALL(sys_cond_signal_to, LV2_NONE);
        syscalls[0x072] = SYSCALL(sys_semaphore_get_value, LV2_NONE);
        syscalls[0x076] = SYSCALL(sys_event_flag_clear, LV2_NONE);
        syscalls[0x080] = SYSCALL(sys_event_queue_create, LV2_NONE);
        syscalls[0x081] = SYSCALL(sys_event_queue_destroy, LV2_NONE);
        syscalls[0x082] = SYSCALL(sys_event_queue_receive, LV2_NONE);
        syscalls[0x084] = SYSCALL(sys_event_flag_cancel, LV2_NONE);
        syscalls[0x086] = SYSCALL(sys_event_port_create, LV2_NONE);
        syscalls[0x087] = SYSCALL(sys_event_port_destroy, LV2_NONE);
        syscalls[0x088] = SYSCALL(sys_event_port_connect_local, LV2_NONE);
        syscalls[0x08B] = SYSCALL(sys_event_flag_get, LV2_NONE);
        syscalls[0x08D] = SYSCALL(sys_timer_usleep, LV2_NONE);
        syscalls[0x08E] = SYSCALL(sys_timer_sleep, LV2_NONE);
        syscalls[0x090] = SYSCALL(sys_time_get_timezone, LV2_NONE);
        syscalls[0x091] = SYSCALL(sys_time_get_current_time, LV2_NONE);
        syscalls[0x093] = SYSCALL(sys_time_get_timebase_frequency, LV2_NONE);
        //syscalls[0x096] = SYSCALL(sys_raw_spu_create_interrupt_tag, LV2_NONE);
        //syscalls[0x097] = SYSCALL(sys_raw_spu_set_int_mask, LV2_NONE);
        //syscalls[0x098] = SYSCALL(sys_raw_spu_get_int_mask, LV2_NONE);
        //syscalls[0x099] = SYSCALL(sys_raw_spu_set_int_stat, LV2_NONE);
        //syscalls[0x09A] = SYSCALL(sys_raw_spu_get_int_stat, LV2_NONE);
        //syscalls[0x09B] = SYSCALL(sys_spu_image_get_information, LV2_NONE);
        //syscalls[0x09C] = SYSCALL(sys_spu_image_open, LV2_NONE);
        //syscalls[0x09D] = SYSCALL(sys_spu_image_import, LV2_NONE);
        //syscalls[0x09E] = SYSCALL(sys_spu_image_close, LV2_NONE);
        //syscalls[0x09F] = SYSCALL(sys_raw_spu_load, LV2_NONE);
        //syscalls[0x0A0] = SYSCALL(sys_raw_spu_create, LV2_NONE);
        //syscalls[0x0A1] = SYSCALL(sys_raw_spu_destroy, LV2_NONE);
        //syscalls[0x0A3] = SYSCALL(sys_raw_spu_read_puint_mb, LV2_NONE);
        //syscalls[0x0A5] = SYSCALL(sys_spu_thread_get_exit_status, LV2_NONE);
        //syscalls[0x0A6] = SYSCALL(sys_spu_thread_set_argument, LV2_NONE);
        //syscalls[0x0A7] = SYSCALL(sys_spu_thread_group_start_on_exit, LV2_NONE);
        syscalls[0x0A9] = SYSCALL(sys_spu_initialize, LV2_NONE);
        syscalls[0x0AA] = SYSCALL(sys_spu_thread_group_create, LV2_NONE);
        syscalls[0x0AB] = SYSCALL(sys_spu_thread_group_destroy, LV2_NONE);
        syscalls[0x0AC] = SYSCALL(sys_spu_thread_initialize, LV2_NONE);
        //syscalls[0x0AD] = SYSCALL(sys_spu_thread_group_start, LV2_NONE);
        //syscalls[0x0AE] = SYSCALL(sys_spu_thread_group_suspend, LV2_NONE);
        //syscalls[0x0AF] = SYSCALL(sys_spu_thread_group_resume, LV2_NONE);
        //syscalls[0x0B0] = SYSCALL(sys_spu_thread_group_yield, LV2_NONE);
        //syscalls[0x0B1] = SYSCALL(sys_spu_thread_group_terminate, LV2_NONE);
        //syscalls[0x0B2] = SYSCALL(sys_spu_thread_group_join, LV2_NONE);
        //syscalls[0x0B3] = SYSCALL(sys_spu_thread_group_set_priority, LV2_NONE);
        //syscalls[0x0B4] = SYSCALL(sys_spu_thread_group_get_priority, LV2_NONE);
        //syscalls[0x0B5] = SYSCALL(sys_spu_thread_write_ls, LV2_NONE);
        //syscalls[0x0B6] = SYSCALL(sys_spu_thread_read_ls, LV2_NONE);
        //syscalls[0x0B8] = SYSCALL(sys_spu_thread_write_snr, LV2_NONE);
        //syscalls[0x0B9] = SYSCALL(sys_spu_thread_group_connect_event, LV2_NONE);
        //syscalls[0x0BA] = SYSCALL(sys_spu_thread_group_disconnect_event, LV2_NONE);
        //syscalls[0x0BB] = SYSCALL(sys_spu_thread_set_spu_cfg, LV2_NONE);
        //syscalls[0x0BC] = SYSCALL(sys_spu_thread_get_spu_cfg, LV2_NONE);
        //syscalls[0x0BE] = SYSCALL(sys_spu_thread_write_spu_mb, LV2_NONE);
        //syscalls[0x0BF] = SYSCALL(sys_spu_thread_connect_event, LV2_NONE);
        //syscalls[0x0C0] = SYSCALL(sys_spu_thread_disconnect_event, LV2_NONE);
        //syscalls[0x0C1] = SYSCALL(sys_spu_thread_bind_queue, LV2_NONE);
        //syscalls[0x0C2] = SYSCALL(sys_spu_thread_unbind_queue, LV2_NONE);
        //syscalls[0x0C4] = SYSCALL(sys_raw_spu_set_spu_cfg, LV2_NONE);
        //syscalls[0x0C5] = SYSCALL(sys_raw_spu_get_spu_cfg, LV2_NONE);
        //syscalls[0x0C6] = SYSCALL(sys_spu_thread_recover_page_fault, LV2_NONE);
        //syscalls[0x0C7] = SYSCALL(sys_raw_spu_recover_page_fault, LV2_NONE);
        syscalls[0x14A] = SYSCALL(sys_mmapper_allocate_address, LV2_NONE);
        syscalls[0x14C] = SYSCALL(sys_mmapper_allocate_shared_memory, LV2_NONE);
        syscalls[0x15C] = SYSCALL(sys_memory_allocate, LV2_NONE);
        syscalls[0x15D] = SYSCALL(sys_memory_free, LV2_NONE);
        syscalls[0x155] = SYSCALL(sys_memory_container_create2, LV2_NONE);
        syscalls[0x15E] = SYSCALL(sys_memory_allocate_from_container, LV2_NONE);
        syscalls[0x15F] = SYSCALL(sys_memory_get_page_attribute, LV2_NONE);
        syscalls[0x160] = SYSCALL(sys_memory_get_user_memory_size, LV2_NONE);
        syscalls[0x161] = SYSCALL(sys_memory_get_user_memory_stat, LV2_NONE);
        syscalls[0x193] = SYSCALL(sys_tty_write, LV2_NONE);
        syscalls[0x1D1] = SYSCALL(sys_prx_load_module_list, LV2_NONE);
        syscalls[0x1E0] = SYSCALL(sys_prx_load_module, LV2_NONE);
        syscalls[0x1E1] = SYSCALL(sys_prx_start_module, LV2_NONE);
        syscalls[0x1E4] = SYSCALL(sys_prx_register_module, LV2_NONE);
        syscalls[0x1E6] = SYSCALL(sys_prx_register_library, LV2_NONE);
        syscalls[0x1EE] = SYSCALL(sys_prx_get_module_list, LV2_NONE);
        syscalls[0x1FE] = SYSCALL(sys_hid_0x1FE, LV2_NONE);
        syscalls[0x200] = SYSCALL(sys_hid_0x200, LV2_NONE);
        syscalls[0x202] = SYSCALL(sys_hid_0x202, LV2_NONE);
        syscalls[0x204] = SYSCALL(sys_config_open, LV2_NONE);
        syscalls[0x205] = SYSCALL(sys_config_close, LV2_NONE);
        syscalls[0x207] = SYSCALL(sys_config_add_service_listener, LV2_NONE);
        syscalls[0x26D] = SYSCALL(sys_gamepad_ycon_if, LV2_NONE);
        syscalls[0x29C] = SYSCALL(sys_rsx_memory_allocate, LV2_NONE);
        syscalls[0x29E] = SYSCALL(sys_rsx_context_allocate, LV2_NONE);
        syscalls[0x2A0] = SYSCALL(sys_rsx_context_iomap, LV2_NONE);
        syscalls[0x2A1] = SYSCALL(sys_rsx_context_iounmap, LV2_NONE);
        syscalls[0x2A2] = SYSCALL(sys_rsx_context_attribute, LV2_NONE);
        syscalls[0x2A3] = SYSCALL(sys_rsx_device_map, LV2_NONE);
        syscalls[0x2A5] = SYSCALL(sys_rsx_attribute, LV2_NONE);
        syscalls[0x321] = SYSCALL(sys_fs_open, LV2_NONE);
        syscalls[0x322] = SYSCALL(sys_fs_read, LV2_NONE);
        syscalls[0x323] = SYSCALL(sys_fs_write, LV2_NONE);
        syscalls[0x324] = SYSCALL(sys_fs_close, LV2_NONE);
        syscalls[0x328] = SYSCALL(sys_fs_stat, LV2_NONE);
        syscalls[0x331] = SYSCALL(sys_fs_fcntl, LV2_NONE);
        syscalls[0x332] = SYSCALL(sys_fs_lseek, LV2_NONE);
        syscalls[0x367] = SYSCALL(sys_ss_access_control_engine, LV2_DBG);
        syscalls[0x3DC] = SYSCALL(sys_dbg_ppu_exception_handler, LV2_NONE);
    }
    if (fw_type & (LV2_DEX | LV2_DECR)) {
        syscalls[0x1CE] = SYSCALL(sys_prx_0x1CE, LV2_NONE);
    }
    if (fw_type & LV2_DECR) {
        // TODO: No syscalls for now
    }

    // Initialize global filesystem devices
    const fs::Path& nucleusPath = fs::AppFileSystem::getPath(fs::APP_LOCATION_LOCAL);
    vfs.registerDevice(new fs::HostPathDevice("/dev_flash", nucleusPath + "platforms/ps3/dev_flash"));
    vfs.registerDevice(new fs::HostPathDevice("/dev_hdd0", nucleusPath + "platforms/ps3/dev_hdd0"));
    vfs.registerDevice(new fs::HostPathDevice("/dev_hdd1", nucleusPath + "platforms/ps3/dev_hdd1"));
    vfs.registerDevice(new fs::HostPathDevice("/host_root/", ""));
}

bool LV2::init(U32 entry) {
    // Load and start liblv2.sprx module
    S32 moduleId = sys_prx_load_module("/dev_flash/sys/external/liblv2.sprx", 0, 0);
    if (moduleId <= CELL_OK) {
        logger.error(LOG_HLE, "You need to provide the /dev_flash/sys/external/ firmware files.");
        return false;
    }
    sys_prx_start_module_option_t startFunc;
    sys_prx_start_module(moduleId, 0, &startFunc);

    BE<U64> thread_id;
    sys_ppu_thread_attr_t attr;
    attr.entry = startFunc.entry.ToLE();
    attr.tls_addr = 0;
    sys_ppu_thread_create(&thread_id, &attr, 0, 0, 500/*TODO*/, 0x10000, 0, "main"/*TODO*/);

    // Set sys_initialize_tls arguments for liblv2.sprx's start function
    auto* state = objects.get<sys_ppu_thread_t>(thread_id)->thread->state.get();
    state->r[7] = 0x0; // TODO
    state->r[8] = 0x0; // TODO
    state->r[9] = 0x0; // TODO
    state->r[10] = 0x90;
    state->r[11] = entry;

    sys_ppu_thread_start(thread_id);
    return true;
}

void LV2::call(cpu::frontend::ppu::PPUState& state) {
    const U32 id = static_cast<U32>(state.r[11]);

    if (!syscalls[id].func) {
        logger.warning(LOG_HLE, "LV2 Syscall %d (0x%x) called", id, id);
        return;
    }
    //logger.notice(LOG_HLE, "LV2 Syscall %d (0x%x: %s) called", id, id, syscalls[id].name);
    syscalls[id].func->call(state, memory->getBaseAddr());
}

}  // namespace sys
