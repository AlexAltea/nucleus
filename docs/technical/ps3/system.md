System Documentation
====================

This document describes the environment in which games and applications run inside the PlayStation 3 and Nucleus approach to emulate it. The PlayStation 3 boot process after FW 3.60 is managed by LV0, which encapsulates all loaders (*appldr*, *isoldr*, *lv1ldr*, *lv2ldr*). LV1, formally *Cell OS Lv-1*, provides the logical partitions, and the resource handling to the underlying operating system. LV2, formally *Cell OS Lv-2*, provides a software execution environment with process management and thread synchronization primitives and communicates with LV1 through `hvsc` or `sc, 1`. Finally, user process can communicate with LV2 through `sc` or `sc, 0`. LV1 code runs in *Hypervisor Mode*, LV2 code runs in *Supervisor Mode* and user processes run in *User Mode*.

Since performance on a PS3 emulator is the biggest challenge, Nucleus is designed as a High-Level Emulator (HLE), providing the User Mode environment by intercepting syscalls and replacing them with a native custom LV2 kernel. This makes LV1 and the booting process irrelevant for us. System libraries are Low-Level Emulated (LLE), that is, dynamically loaded into memory, linked and run as part of the game.

A few points to take into account are:

* Games and applications cannot spawn multiple processes, therefore Nucleus focuses in emulating **one process**. However, games can use Inter-Process Communication (IPC) with system process like *vsh.self* (e.g. via the *libsysutil.sprx* module). An approach for this is explained below.

* To deal with performance issues in hardware intensive system modules (e.g. image/audio/video decoding modules), or bypassing IPC queries (rarely used by the application directly), **some system library functions are HLE'd** instead of LLE'd.

* For the sake of resembling a bit more the original PS3, some RSX-related LV2 syscalls have their implementation splitted in the *syscalls/lv1* and *syscalls/lv2* folders. For now, there is no further meaning in this.
