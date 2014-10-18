/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv2.h"
#include "nucleus/memory/object.h"
#include "nucleus/syscalls/callback.h"

#include "lv2/sys_cond.h"
#include "lv2/sys_dbg.h"
#include "lv2/sys_fs.h"
#include "lv2/sys_lwmutex.h"
#include "lv2/sys_memory.h"
#include "lv2/sys_mmapper.h"
#include "lv2/sys_mutex.h"
#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"
#include "lv2/sys_rsx.h"
#include "lv2/sys_semaphore.h"
#include "lv2/sys_ss.h"
#include "lv2/sys_time.h"
#include "lv2/sys_tty.h"

LV2::LV2(u32 fw_type)
{
    // Initialize syscall table
    if (fw_type & (LV2_CEX | LV2_DEX | LV2_DECR)) {
        m_syscalls[0x001] = {wrap(sys_process_getpid), LV2_NONE};
        m_syscalls[0x019] = {wrap(sys_process_get_sdk_version), LV2_NONE};
        m_syscalls[0x003] = {wrap(sys_process_exit), LV2_NONE};
        m_syscalls[0x016] = {wrap(sys_process_exit), LV2_NONE};
        m_syscalls[0x01E] = {wrap(sys_process_get_paramsfo), LV2_NONE};       
        m_syscalls[0x05A] = {wrap(sys_semaphore_create), LV2_NONE};
        m_syscalls[0x05B] = {wrap(sys_semaphore_destroy), LV2_NONE};
        m_syscalls[0x05C] = {wrap(sys_semaphore_wait), LV2_NONE};
        m_syscalls[0x05D] = {wrap(sys_semaphore_trywait), LV2_NONE};
        m_syscalls[0x05E] = {wrap(sys_semaphore_post), LV2_NONE};
        m_syscalls[0x05F] = {wrap(sys_lwmutex_create), LV2_NONE};
        m_syscalls[0x060] = {wrap(sys_lwmutex_destroy), LV2_NONE};
        m_syscalls[0x061] = {wrap(sys_lwmutex_lock), LV2_NONE};
        m_syscalls[0x062] = {wrap(sys_lwmutex_trylock), LV2_NONE};
        m_syscalls[0x063] = {wrap(sys_lwmutex_unlock), LV2_NONE};
        m_syscalls[0x064] = {wrap(sys_mutex_create), LV2_NONE};
        m_syscalls[0x065] = {wrap(sys_mutex_destroy), LV2_NONE};
        m_syscalls[0x066] = {wrap(sys_mutex_lock), LV2_NONE};
        m_syscalls[0x067] = {wrap(sys_mutex_trylock), LV2_NONE};
        m_syscalls[0x068] = {wrap(sys_mutex_unlock), LV2_NONE};
        m_syscalls[0x069] = {wrap(sys_cond_create), LV2_NONE};
        m_syscalls[0x06A] = {wrap(sys_cond_destroy), LV2_NONE};
        m_syscalls[0x06B] = {wrap(sys_cond_wait), LV2_NONE};
        m_syscalls[0x06C] = {wrap(sys_cond_signal), LV2_NONE};
        m_syscalls[0x06D] = {wrap(sys_cond_signal_all), LV2_NONE};
        m_syscalls[0x06E] = {wrap(sys_cond_signal_to), LV2_NONE};
        m_syscalls[0x072] = {wrap(sys_semaphore_get_value), LV2_NONE};
        m_syscalls[0x090] = {wrap(sys_time_get_timezone), LV2_NONE};
        m_syscalls[0x091] = {wrap(sys_time_get_current_time), LV2_NONE};
        m_syscalls[0x093] = {wrap(sys_time_get_timebase_frequency), LV2_NONE};
        m_syscalls[0x14A] = {wrap(sys_mmapper_allocate_address), LV2_NONE};
        m_syscalls[0x14C] = {wrap(sys_mmapper_allocate_shared_memory), LV2_NONE};
        m_syscalls[0x15C] = {wrap(sys_memory_allocate), LV2_NONE};
        m_syscalls[0x15D] = {wrap(sys_memory_free), LV2_NONE};
        m_syscalls[0x155] = {wrap(sys_memory_container_create2), LV2_NONE};
        m_syscalls[0x15E] = {wrap(sys_memory_allocate_from_container), LV2_NONE};
        m_syscalls[0x15F] = {wrap(sys_memory_get_page_attribute), LV2_NONE};
        m_syscalls[0x160] = {wrap(sys_memory_get_user_memory_size), LV2_NONE};
        m_syscalls[0x161] = {wrap(sys_memory_get_user_memory_stat), LV2_NONE};
        m_syscalls[0x193] = {wrap(sys_tty_write), LV2_NONE};
        m_syscalls[0x1D1] = {wrap(sys_prx_load_module_list), LV2_NONE};
        m_syscalls[0x1E0] = {wrap(sys_prx_load_module), LV2_NONE};
        m_syscalls[0x1E1] = {wrap(sys_prx_start_module), LV2_NONE};
        m_syscalls[0x1E4] = {wrap(sys_prx_register_module), LV2_NONE};
        m_syscalls[0x1E6] = {wrap(sys_prx_register_library), LV2_NONE};
        m_syscalls[0x1EE] = {wrap(sys_prx_get_module_list), LV2_NONE};
        m_syscalls[0x29C] = {wrap(sys_rsx_memory_allocate), LV2_NONE};
        m_syscalls[0x29E] = {wrap(sys_rsx_context_allocate), LV2_NONE};
        m_syscalls[0x2A3] = {wrap(sys_rsx_device_map), LV2_NONE};
        m_syscalls[0x321] = {wrap(sys_fs_open), LV2_NONE};
        m_syscalls[0x323] = {wrap(sys_fs_read), LV2_NONE};
        m_syscalls[0x323] = {wrap(sys_fs_write), LV2_NONE};
        m_syscalls[0x324] = {wrap(sys_fs_close), LV2_NONE};
        m_syscalls[0x367] = {wrap(sys_ss_access_control_engine), LV2_DBG};
        m_syscalls[0x3DC] = {wrap(sys_dbg_ppu_exception_handler), LV2_NONE};
    }
    if (fw_type & (LV2_DEX | LV2_DECR)) {
        m_syscalls[0x1CE] = {wrap(sys_prx_1CE), LV2_NONE};
    }
    if (fw_type & LV2_DECR) {
        // TODO: No syscalls for now
    }
}

bool LV2::init()
{
    // Load and start liblv2.sprx module
    s32 moduleId = sys_prx_load_module("/dev_flash/sys/external/liblv2.sprx", 0, 0);
    if (moduleId <= CELL_OK) {
        nucleus.log.error(LOG_HLE, "You need to provide the /dev_flash/sys/external/ firmware files.");
        return false;
    }
    sys_prx_start_module_option_t startFunc;
    sys_prx_start_module(moduleId, 0, &startFunc);
    Callback{(u32)startFunc.entry}.call();
    return true;
}

void LV2::call(PPUThread& thread)
{
    const u32 id = thread.gpr[11];

    if (!m_syscalls[id].func) {
        nucleus.log.warning(LOG_HLE, "LV2 Syscall %d (0x%x) called", id, id);
        return;
    }

    m_syscalls[id].func->call(thread, nucleus.memory.getBaseAddr());
}
