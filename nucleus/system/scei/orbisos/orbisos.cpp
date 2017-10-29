/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "orbisos.h"

#include "syscalls/orbis_dynlib.h"

#define SYSCALL(name) { #name }

namespace sys {
namespace scei {
namespace orbis {

OrbisOS::OrbisOS(Emulator* emulator)
    : System(emulator)
{
    // Add FreeBSD 9 syscalls referenced in libkernel.sprx (FW 1.62)
    //syscalls[0x000] = SYSCALL();
    //syscalls[0x001] = SYSCALL(sys_exit);
    //syscalls[0x002] = SYSCALL(sys_fork);
    //syscalls[0x003] = SYSCALL(sys_read);
    //syscalls[0x004] = SYSCALL(sys_write);
    //syscalls[0x005] = SYSCALL(sys_open);
    //syscalls[0x006] = SYSCALL(sys_close);
    //syscalls[0x007] = SYSCALL(sys_wait4);
    //syscalls[0x009] = SYSCALL(sys_link);
    //syscalls[0x00A] = SYSCALL(sys_unlink);
    //syscalls[0x00C] = SYSCALL(sys_chdir);
    //syscalls[0x00D] = SYSCALL(sys_fchdir);
    //syscalls[0x00E] = SYSCALL(sys_mknod);
    //syscalls[0x00F] = SYSCALL(sys_chmod);
    //syscalls[0x010] = SYSCALL(sys_chown);
    //syscalls[0x014] = SYSCALL(sys_getpid);
    //syscalls[0x015] = SYSCALL(sys_mount);
    //syscalls[0x016] = SYSCALL(sys_umount);
    //syscalls[0x017] = SYSCALL(sys_setuid);
    //syscalls[0x018] = SYSCALL(sys_getuid);
    //syscalls[0x019] = SYSCALL(sys_geteuid);
    //syscalls[0x01B] = SYSCALL(sys_recvmsg);
    //syscalls[0x01C] = SYSCALL(sys_sendmsg);
    //syscalls[0x01D] = SYSCALL(sys_recvfrom);
    //syscalls[0x01E] = SYSCALL(sys_accept);
    //syscalls[0x01F] = SYSCALL(sys_getpeername);
    //syscalls[0x020] = SYSCALL(sys_getsockname);
    //syscalls[0x021] = SYSCALL(sys_access);
    //syscalls[0x022] = SYSCALL(sys_chflags);
    //syscalls[0x023] = SYSCALL(sys_fchflags);
    //syscalls[0x024] = SYSCALL(sys_sync);
    //syscalls[0x025] = SYSCALL(sys_kill);
    //syscalls[0x027] = SYSCALL(sys_getppid);
    //syscalls[0x029] = SYSCALL(sys_dup);
    //syscalls[0x02B] = SYSCALL(sys_getegid);
    //syscalls[0x02C] = SYSCALL(sys_profile);
    //syscalls[0x02D] = SYSCALL(sys_ktrace);
    //syscalls[0x02F] = SYSCALL(sys_getgid);
    //syscalls[0x031] = SYSCALL(sys_getlogin);
    //syscalls[0x035] = SYSCALL(sys_sigaltstack);
    //syscalls[0x036] = SYSCALL(sys_ioctl);
    //syscalls[0x038] = SYSCALL(sys_revoke);
    //syscalls[0x039] = SYSCALL(sys_symlink);
    //syscalls[0x03A] = SYSCALL(sys_readlink);
    //syscalls[0x03B] = SYSCALL(sys_execve);
    //syscalls[0x03C] = SYSCALL(sys_umask);
    //syscalls[0x03D] = SYSCALL(sys_chroot);
    //syscalls[0x041] = SYSCALL(sys_msync);
    //syscalls[0x049] = SYSCALL(sys_munmap);
    //syscalls[0x04A] = SYSCALL(sys_mprotect);
    //syscalls[0x04B] = SYSCALL(sys_madvise);
    //syscalls[0x04E] = SYSCALL(sys_mincore);
    //syscalls[0x04F] = SYSCALL(sys_getgroups);
    //syscalls[0x050] = SYSCALL(sys_setgroups);
    //syscalls[0x052] = SYSCALL(sys_setpgrp);
    //syscalls[0x053] = SYSCALL(sys_setitimer);
    //syscalls[0x055] = SYSCALL(sys_swapon);
    //syscalls[0x056] = SYSCALL(sys_getitimer);
    //syscalls[0x059] = SYSCALL(sys_getdtablesize);
    //syscalls[0x05A] = SYSCALL(sys_dup2);
    //syscalls[0x05C] = SYSCALL(sys_fcntl);
    //syscalls[0x05D] = SYSCALL(sys_select);
    //syscalls[0x05F] = SYSCALL(sys_fsync);
    //syscalls[0x060] = SYSCALL(sys_setpriority);
    //syscalls[0x061] = SYSCALL(sys_socket);
    //syscalls[0x062] = SYSCALL(sys_connect);
    //syscalls[0x064] = SYSCALL(sys_getpriority);
    //syscalls[0x068] = SYSCALL(sys_bind);
    //syscalls[0x069] = SYSCALL(sys_setsockopt);
    //syscalls[0x06A] = SYSCALL(sys_listen);
    //syscalls[0x074] = SYSCALL(sys_gettimeofday);
    //syscalls[0x075] = SYSCALL(sys_getrusage);
    //syscalls[0x076] = SYSCALL(sys_getsockopt);
    //syscalls[0x078] = SYSCALL(sys_readv);
    //syscalls[0x079] = SYSCALL(sys_writev);
    //syscalls[0x07A] = SYSCALL(sys_settimeofday);
    //syscalls[0x07B] = SYSCALL(sys_fchown);
    //syscalls[0x07C] = SYSCALL(sys_fchmod);
    //syscalls[0x07E] = SYSCALL(sys_setreuid);
    //syscalls[0x07F] = SYSCALL(sys_setregid);
    //syscalls[0x080] = SYSCALL(sys_rename);
    //syscalls[0x083] = SYSCALL(sys_flock);
    //syscalls[0x084] = SYSCALL(sys_mkfifo);
    //syscalls[0x085] = SYSCALL(sys_sendto);
    //syscalls[0x086] = SYSCALL(sys_shutdown);
    //syscalls[0x087] = SYSCALL(sys_socketpair);
    //syscalls[0x088] = SYSCALL(sys_mkdir);
    //syscalls[0x089] = SYSCALL(sys_rmdir);
    //syscalls[0x08A] = SYSCALL(sys_utimes);
    //syscalls[0x08C] = SYSCALL(sys_adjtime);
    //syscalls[0x093] = SYSCALL(sys_setsid);
    //syscalls[0x0A5] = SYSCALL(sys_sysarch);
    //syscalls[0x0B6] = SYSCALL(sys_setegid);
    //syscalls[0x0B7] = SYSCALL(sys_seteuid);
    //syscalls[0x0BC] = SYSCALL(sys_stat);
    //syscalls[0x0BD] = SYSCALL(sys_fstat);
    //syscalls[0x0BE] = SYSCALL(sys_lstat);
    //syscalls[0x0BF] = SYSCALL(sys_pathconf);
    //syscalls[0x0C0] = SYSCALL(sys_fpathconf);
    //syscalls[0x0C2] = SYSCALL(sys_getrlimit);
    //syscalls[0x0C3] = SYSCALL(sys_setrlimit);
    //syscalls[0x0C4] = SYSCALL(sys_getdirentries);
    //syscalls[0x0CA] = SYSCALL(sys_sysctl);
    //syscalls[0x0CB] = SYSCALL(sys_mlock);
    //syscalls[0x0CC] = SYSCALL(sys_munlock);
    //syscalls[0x0CE] = SYSCALL(sys_futimes);
    //syscalls[0x0D1] = SYSCALL(sys_poll);
    //syscalls[0x0E8] = SYSCALL(sys_clock_gettime);
    //syscalls[0x0E9] = SYSCALL(sys_clock_settime);
    //syscalls[0x0EA] = SYSCALL(sys_clock_getres);
    //syscalls[0x0EB] = SYSCALL(sys_ktimer_create);
    //syscalls[0x0EC] = SYSCALL(sys_ktimer_delete);
    //syscalls[0x0ED] = SYSCALL(sys_ktimer_settime);
    //syscalls[0x0EE] = SYSCALL(sys_ktimer_gettime);
    //syscalls[0x0EF] = SYSCALL(sys_ktimer_getoverrun);
    //syscalls[0x0F0] = SYSCALL(sys_nanosleep);
    //syscalls[0x0FD] = SYSCALL(sys_issetugid);
    //syscalls[0x0FE] = SYSCALL(sys_lchown);
    //syscalls[0x0FF] = SYSCALL(sys_aio_read);
    //syscalls[0x100] = SYSCALL(sys_aio_write);
    //syscalls[0x110] = SYSCALL(sys_getdents);
    //syscalls[0x112] = SYSCALL(sys_lchmod);
    //syscalls[0x114] = SYSCALL(sys_lutimes);
    //syscalls[0x121] = SYSCALL(sys_preadv);
    //syscalls[0x122] = SYSCALL(sys_pwritev);
    //syscalls[0x130] = SYSCALL(sys_kldload);
    //syscalls[0x131] = SYSCALL(sys_kldunload);
    //syscalls[0x132] = SYSCALL(sys_kldfind);
    //syscalls[0x133] = SYSCALL(sys_kldnext);
    //syscalls[0x134] = SYSCALL(sys_kldstat);
    //syscalls[0x135] = SYSCALL(sys_kldfirstmod);
    //syscalls[0x136] = SYSCALL(sys_getsid);
    //syscalls[0x13A] = SYSCALL(sys_aio_return);
    //syscalls[0x13B] = SYSCALL(sys_aio_suspend);
    //syscalls[0x13C] = SYSCALL(sys_aio_cancel);
    //syscalls[0x13D] = SYSCALL(sys_aio_error);
    //syscalls[0x144] = SYSCALL(sys_mlockall);
    //syscalls[0x145] = SYSCALL(sys_munlockall);
    //syscalls[0x146] = SYSCALL(sys_getcwd);
    //syscalls[0x147] = SYSCALL(sys_sched_setparam);
    //syscalls[0x148] = SYSCALL(sys_sched_getparam);
    //syscalls[0x149] = SYSCALL(sys_sched_setscheduler);
    //syscalls[0x14A] = SYSCALL(sys_sched_getscheduler);
    //syscalls[0x14B] = SYSCALL(sys_sched_yield);
    //syscalls[0x14C] = SYSCALL(sys_sched_get_priority_max);
    //syscalls[0x14D] = SYSCALL(sys_sched_get_priority_min);
    //syscalls[0x14E] = SYSCALL(sys_sched_rr_get_interval);
    //syscalls[0x14F] = SYSCALL(sys_utrace);
    //syscalls[0x151] = SYSCALL(sys_kldsym);
    //syscalls[0x154] = SYSCALL(sys_sigprocmask);
    //syscalls[0x155] = SYSCALL(sys_sigsuspend);
    //syscalls[0x157] = SYSCALL(sys_sigpending);
    //syscalls[0x159] = SYSCALL(sys_sigtimedwait);
    //syscalls[0x15A] = SYSCALL(sys_sigwaitinfo);
    //syscalls[0x167] = SYSCALL(sys_aio_waitcomplete);
    //syscalls[0x16A] = SYSCALL(sys_kqueue);
    //syscalls[0x16B] = SYSCALL(sys_kevent);
    //syscalls[0x179] = SYSCALL(sys_afs3_syscall);
    //syscalls[0x17A] = SYSCALL(sys_nmount);
    //syscalls[0x180] = SYSCALL(sys_mac_get_proc);
    //syscalls[0x181] = SYSCALL(sys_mac_set_proc);
    //syscalls[0x182] = SYSCALL(sys_mac_get_fd);
    //syscalls[0x183] = SYSCALL(sys_mac_get_file);
    //syscalls[0x184] = SYSCALL(sys_mac_set_fd);
    //syscalls[0x185] = SYSCALL(sys_mac_set_file);
    //syscalls[0x186] = SYSCALL(sys_kenv);
    //syscalls[0x187] = SYSCALL(sys_lchflags);
    //syscalls[0x188] = SYSCALL(sys_uuidgen);
    //syscalls[0x189] = SYSCALL(sys_sendfile);
    //syscalls[0x18B] = SYSCALL(sys_getfsstat);
    //syscalls[0x18C] = SYSCALL(sys_statfs);
    //syscalls[0x18D] = SYSCALL(sys_fstatfs);
    //syscalls[0x190] = SYSCALL(sys_ksem_close);
    //syscalls[0x191] = SYSCALL(sys_ksem_post);
    //syscalls[0x192] = SYSCALL(sys_ksem_wait);
    //syscalls[0x193] = SYSCALL(sys_ksem_trywait);
    //syscalls[0x194] = SYSCALL(sys_ksem_init);
    //syscalls[0x195] = SYSCALL(sys_ksem_open);
    //syscalls[0x196] = SYSCALL(sys_ksem_unlink);
    //syscalls[0x197] = SYSCALL(sys_ksem_getvalue);
    //syscalls[0x198] = SYSCALL(sys_ksem_destroy);
    //syscalls[0x199] = SYSCALL(sys_mac_get_pid);
    //syscalls[0x19A] = SYSCALL(sys_mac_get_link);
    //syscalls[0x19B] = SYSCALL(sys_mac_set_link);
    //syscalls[0x19F] = SYSCALL(sys_max_execve);
    //syscalls[0x1A0] = SYSCALL(sys_sigaction);
    //syscalls[0x1A6] = SYSCALL(sys_setcontext);
    //syscalls[0x1A7] = SYSCALL(sys_swapcontext);
    //syscalls[0x1AD] = SYSCALL(sys_sigwait);
    //syscalls[0x1AE] = SYSCALL(sys_thr_create);
    //syscalls[0x1AF] = SYSCALL(sys_thr_exit);
    //syscalls[0x1B0] = SYSCALL(sys_thr_self);
    //syscalls[0x1B1] = SYSCALL(sys_thr_kill);
    //syscalls[0x1B2] = SYSCALL(sys_umtx_lock);
    //syscalls[0x1B3] = SYSCALL(sys_umtx_unlock);
    //syscalls[0x1B9] = SYSCALL(sys_ksem_timedwait);
    //syscalls[0x1BA] = SYSCALL(sys_thr_suspend);
    //syscalls[0x1BB] = SYSCALL(sys_thr_wake);
    //syscalls[0x1BC] = SYSCALL(sys_kldunloadf);
    //syscalls[0x1C6] = SYSCALL(sys_umtx_op);
    //syscalls[0x1C7] = SYSCALL(sys_thr_new);
    //syscalls[0x1C8] = SYSCALL(sys_sigqueue);
    //syscalls[0x1C9] = SYSCALL(sys_kmq_open);
    //syscalls[0x1CA] = SYSCALL(sys_kmq_setattr);
    //syscalls[0x1CB] = SYSCALL(sys_kmq_timedreceive);
    //syscalls[0x1CC] = SYSCALL(sys_kmq_timedsend);
    //syscalls[0x1CD] = SYSCALL(sys_kmq_notify);
    //syscalls[0x1CE] = SYSCALL(sys_kmq_unlink);
    //syscalls[0x1D0] = SYSCALL(sys_thr_set_name);
    //syscalls[0x1D1] = SYSCALL(sys_aio_fsync);
    //syscalls[0x1D2] = SYSCALL(sys_rtprio_thread);
    //syscalls[0x1DB] = SYSCALL(sys_pread);
    //syscalls[0x1DC] = SYSCALL(sys_pwrite);
    //syscalls[0x1DD] = SYSCALL(sys_mmap);
    //syscalls[0x1DE] = SYSCALL(sys_lseek);
    //syscalls[0x1DF] = SYSCALL(sys_truncate);
    //syscalls[0x1E0] = SYSCALL(sys_ftruncate);
    //syscalls[0x1E1] = SYSCALL(sys_thr_kill2);
    //syscalls[0x1E2] = SYSCALL(sys_shm_open);
    //syscalls[0x1E3] = SYSCALL(sys_shm_unlink);
    //syscalls[0x1E4] = SYSCALL(sys_cpuset);
    //syscalls[0x1E5] = SYSCALL(sys_cpuset_setid);
    //syscalls[0x1E6] = SYSCALL(sys_cpuset_getid);
    //syscalls[0x1E7] = SYSCALL(sys_cpuset_getaffinity);
    //syscalls[0x1E8] = SYSCALL(sys_cpuset_setaffinity);
    //syscalls[0x1EA] = SYSCALL(sys_fchmodat);
    //syscalls[0x1EB] = SYSCALL(sys_fchownat);
    //syscalls[0x1ED] = SYSCALL(sys_fstatat);
    //syscalls[0x1EE] = SYSCALL(sys_futimesat);
    //syscalls[0x1EF] = SYSCALL(sys_linkat);
    //syscalls[0x1F0] = SYSCALL(sys_mkdirat);
    //syscalls[0x1F1] = SYSCALL(sys_mkfifoat);
    //syscalls[0x1F2] = SYSCALL(sys_mknodat);
    //syscalls[0x1F3] = SYSCALL(sys_openat);
    //syscalls[0x1F5] = SYSCALL(sys_renameat);
    //syscalls[0x1F6] = SYSCALL(sys_symlinkat);
    //syscalls[0x1F7] = SYSCALL(sys_unlinkat);
    //syscalls[0x204] = SYSCALL(sys_cap_enter);
    //syscalls[0x205] = SYSCALL(sys_cap_getmode);
    //syscalls[0x20A] = SYSCALL(sys_pselect);

    // Add custom SCE syscalls referenced in libkernel.sprx (FW 1.62)
    //syscalls[0x22F] = SYSCALL(sys_set_vm_container);
    //syscalls[0x230] = SYSCALL(sys_debug_init);
    //syscalls[0x233] = SYSCALL(sys_opmc_enable);
    //syscalls[0x234] = SYSCALL(sys_opmc_disable);
    //syscalls[0x235] = SYSCALL(sys_opmc_set_ctl);
    //syscalls[0x236] = SYSCALL(sys_opmc_set_ctr);
    //syscalls[0x237] = SYSCALL(sys_opmc_get_ctr);
    //syscalls[0x23C] = SYSCALL(sys_virtual_query);
    //syscalls[0x23D] = SYSCALL(sys_mdbg_call); // Disabled ?
    //syscalls[0x244] = SYSCALL(sys_eport_create);
    //syscalls[0x245] = SYSCALL(sys_eport_delete);
    //syscalls[0x246] = SYSCALL(sys_eport_trigger);
    //syscalls[0x247] = SYSCALL(sys_eport_open);
    //syscalls[0x248] = SYSCALL(sys_eport_close);
    //syscalls[0x249] = SYSCALL(sys_is_in_sandbox);
    //syscalls[0x24A] = SYSCALL(sys_dmem_container);
    //syscalls[0x24B] = SYSCALL(sys_get_authinfo);
    //syscalls[0x24C] = SYSCALL(sys_mname);
    //syscalls[0x24D] = SYSCALL(sys_dynlib_dlopen);
    //syscalls[0x24E] = SYSCALL(sys_dynlib_dlclose);
    //syscalls[0x24F] = SYSCALL(sys_dynlib_dlsym);
    //syscalls[0x250] = SYSCALL(sys_dynlib_get_list);
    //syscalls[0x251] = SYSCALL(sys_dynlib_get_info);
    syscalls[0x252] = SYSCALL(sys_dynlib_load_prx);
    //syscalls[0x253] = SYSCALL(sys_dynlib_unload_prx);
    //syscalls[0x254] = SYSCALL(sys_dynlib_do_copy_relocations);
    //syscalls[0x255] = SYSCALL(sys_dynlib_prepare_dlclose);
    //syscalls[0x256] = SYSCALL(sys_dynlib_get_proc_param);
    //syscalls[0x257] = SYSCALL(sys_dynlib_process_needed_and_relocate);
    //syscalls[0x258] = SYSCALL(sys_sandbox_path);
    //syscalls[0x259] = SYSCALL(sys_mdbg_service);
    //syscalls[0x25A] = SYSCALL(sys_randomized_path);
    //syscalls[0x25B] = SYSCALL(sys_rdup);
    //syscalls[0x25C] = SYSCALL(sys_dl_get_metadata);
    //syscalls[0x25E] = SYSCALL(sys_is_development_mode);
    //syscalls[0x25F] = SYSCALL(sys_get_self_auth_info);
    //syscalls[0x260] = SYSCALL(sys_dynlib_get_info_ex);
    //syscalls[0x261] = SYSCALL(sys_budget_getid); // Disabled?
    //syscalls[0x262] = SYSCALL(sys_budget_get_ptype); // Disabled?
    //syscalls[0x264] = SYSCALL(sys_get_proc_type_info);
    //syscalls[0x265] = SYSCALL(sys_get_resident_count);
    //syscalls[0x266] = SYSCALL(sys_prepare_to_suspend_process);
    //syscalls[0x267] = SYSCALL(sys_get_resident_fmem_count);

    // Initialize sandbox directories
    vfs.registerDevice(new fs::HostPathDevice("/host/", ""));
}

bool OrbisOS::init(U64 entry) {
    // Load libkernel.sprx and run its start function, which will load
    // the remaining modules and jump to the executable entry point
    dynlib_load_prx_args args = {};
    args.path = "/system/common/lib/libkernel.sprx";
    return !sys_dynlib_load_prx(nullptr, &args);
}

}  // namespace orbis
}  // namespace scei
}  // namespace sys
