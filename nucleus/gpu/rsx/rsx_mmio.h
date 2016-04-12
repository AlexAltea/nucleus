/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

/**
 * RSX MMIO
 * ========
 * The following offsets point into the 32 MB MMIO register space available at BAR0.
 * The entirety BAR0 or ranges of it can be mapped into virtual address space through the
 * `sys_rsx_device_map` CellOS-LV2 syscall, which relies on the
 * `lv1_gpu_device_map` CellOS-LV1 syscall.
 *
 * Each of the MMIO register ranges mappable by these syscalls correspond to different
 * "devices" (SCEI nomenclature) or "engines" (Nouveau nomenclature).
 *
 * The following table lists accessible RSX devices:
 * +------------+-------------------+--------------+
 * | Offset     | Size              | Device       |
 * |------------|-------------------|--------------|
 * | 0x00002000 | 0x0002000 (8 KB)  | PFIFO        |
 * | 0x0000A000 | 0x0001000 (4 KB)  | PCOUNTER     |
 * | 0x00080100 | 0x0008000 (32 KB) | ?            |
 * | 0x00088000 | 0x0001000 (4 KB)  | PPCI         |
 * | 0x0008A000 | 0x0001000 (4 KB)  | ?            |
 * | 0x00090000 | 0x0001000 (4 KB)  | PFIFO_CACHE? |
 * | 0x00100000 | 0x0001000 (4 KB)  | PFB          |
 * | 0x00200000 | 0x0001000 (4 KB)  | PMEDIA?      |
 * | 0x00400000 | 0x0008000 (32 KB) | PGRAPH       |
 * | 0x00600000 | 0x0010000 (64 KB) | PCRTC?       |
 * +------------+-------------------+--------------+
 */

// PFIFO
#define RSX_PFIFO_DELAY_0                                 0x00002040
#define RSX_PFIFO_DMA_TIMESLICE                           0x00002044
#define RSX_PFIFO_UNK2048                                 0x00002048
#define RSX_PFIFO_UNK204C                                 0x0000204C
#define RSX_PFIFO_NEXT_CHANNEL                            0x00002050
#define RSX_PFIFO_UNK2054                                 0x00002054
#define RSX_PFIFO_UNK2058                                 0x00002058 // NV36 Ch DMA
#define RSX_PFIFO_UNK2088                                 0x00002088
#define RSX_PFIFO_INTR_0                                  0x00002100 // TODO: Do the fields below match RSX_PFIFO_INTR_EN_0's ones due to a mistake?
#define     RSX_PFIFO_INTR_0_ACQUIRE_TIMEOUT    // [06:06], 1(intr acquire timeout)
#define     RSX_PFIFO_INTR_0_FBI_STATE_TIMEOUT  // [08:08], 1(intr FBI state timeout)
#define     RSX_PFIFO_INTR_0_FBI_READ_TIMEOUT   // [09:09], 1(intr FBI read timeout)
#define     RSX_PFIFO_INTR_0_BIT10              // [10:10], ?
#define     RSX_PFIFO_INTR_0_DMA_PUT            // [11:11], 1(intr DMA PUT)
#define     RSX_PFIFO_INTR_0_MARKER_ERROR       // [12:12], 1(marker error)
#define     RSX_PFIFO_INTR_0_DMA_PUSHER         // [18:18], 1(intr DMA pusher)
#define     RSX_PFIFO_INTR_0_RUNOUT_OVERFLOW    // [22:22], 1(intr runout overflow)
#define RSX_PFIFO_INTR_EN_0                               0x00002140
#define     NV_PFIFO_INTR_CACHE_ERROR                       (1 <<  0)
#define     NV_PFIFO_INTR_RUNOUT                            (1 <<  4)
#define     NV_PFIFO_INTR_RUNOUT_OVERFLOW                   (1 <<  8)
#define     NV_PFIFO_INTR_DMA_PUSHER                        (1 << 12)
#define     NV_PFIFO_INTR_DMA_PT                            (1 << 16)
#define     NV_PFIFO_INTR_SEMAPHORE                         (1 << 20)
#define     NV_PFIFO_INTR_ACQUIRE_TIMEOUT                   (1 << 24)
#define RSX_PFIFO_RAMHT                                   0x00002210 // Offset must be 4 KB aligned and in range 0x0 to 0x1F000
#define RSX_PFIFO_UNK2214                                 0x00002214
#define RSX_PFIFO_RAMRO                                   0x00002218 // Offset must be 512 Byte aligned and in range 0x0 to 0x1FE00
#define RSX_PFIFO_UNK221C                                 0x0000221C
#define RSX_PFIFO_RAMFC                                   0x00002220
#define RSX_PFIFO_UNK2224                                 0x00002224
#define RSX_PFIFO_UNK2228                                 0x00002228
#define RSX_PFIFO_UNK222C                                 0x0000222C
#define RSX_PFIFO_UNK2230                                 0x00002230
#define RSX_PFIFO_UNK2410                                 0x00002410
#define RSX_PFIFO_UNK2420                                 0x00002420
#define RSX_PFIFO_CACHES                                  0x00002500
#define RSX_PFIFO_MODE                                    0x00002504
#define RSX_PFIFO_DMA                                     0x00002508
#define RSX_PFIFO_SIZE                                    0x0000250C // LV1 does weird things here
// TODO: Every word from 0x2510 to 0x254C is set to 0 on LV1
#define RSX_PFIFO_CACHE1_PUSH0                            0x00003200
#define RSX_PFIFO_CACHE1_PUSH1                            0x00003204 // TODO: Check CHID mask
#define RSX_PFIFO_CACHE1_PUT                              0x00003210
#define RSX_PFIFO_CACHE1_DMA_PUSH                         0x00003220
#define RSX_PFIFO_CACHE1_DMA_FETCH                        0x00003224
#define     RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_MASK             0x000000FF
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_8_BYTES      0x00000000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_16_BYTES     0x00000008
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_24_BYTES     0x00000010
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_32_BYTES     0x00000018
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_40_BYTES     0x00000020
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_48_BYTES     0x00000028
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_56_BYTES     0x00000030
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_64_BYTES     0x00000038
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_72_BYTES     0x00000040
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_80_BYTES     0x00000048
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_88_BYTES     0x00000050
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_96_BYTES     0x00000058
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_104_BYTES    0x00000060
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_112_BYTES    0x00000068
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_120_BYTES    0x00000070
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_128_BYTES    0x00000078
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_136_BYTES    0x00000080
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_144_BYTES    0x00000088
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_152_BYTES    0x00000090
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_160_BYTES    0x00000098
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_168_BYTES    0x000000A0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_176_BYTES    0x000000A8
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_184_BYTES    0x000000B0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_192_BYTES    0x000000B8
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_200_BYTES    0x000000C0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_208_BYTES    0x000000C8
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_216_BYTES    0x000000D0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_224_BYTES    0x000000D8
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_232_BYTES    0x000000E0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_240_BYTES    0x000000E8
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_248_BYTES    0x000000F0
#define         RSX_PFIFO_CACHE1_DMA_FETCH_TRIG_256_BYTES    0x000000F8
#define     RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_MASK             0x0000E000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_32_BYTES     0x00000000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_64_BYTES     0x00002000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_96_BYTES     0x00004000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_128_BYTES    0x00006000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_160_BYTES    0x00008000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_192_BYTES    0x0000A000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_224_BYTES    0x0000C000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_SIZE_256_BYTES    0x0000E000
#define     RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS              0x001F0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_0        0x00000000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_1        0x00010000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_2        0x00020000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_3        0x00030000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_4        0x00040000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_5        0x00050000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_6        0x00060000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_7        0x00070000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_8        0x00080000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_9        0x00090000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_10       0x000A0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_11       0x000B0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_12       0x000C0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_13       0x000D0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_14       0x000E0000
#define         RSX_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_15       0x000F0000
#define     RSX_PFIFO_CACHE1_ENDIAN                          0x80000000
#define         RSX_PFIFO_CACHE1_LITTLE_ENDIAN               0x7FFFFFFF
#define         RSX_PFIFO_CACHE1_BIG_ENDIAN                  0x80000000
#define RSX_PFIFO_CACHE1_DMA_STATE                      0x00003228
#define RSX_PFIFO_CACHE1_DMA_INSTANCE                   0x0000322c
#define RSX_PFIFO_CACHE1_DMA_CTL                        0x00003230
#define RSX_PFIFO_CACHE1_DMA_PUT                        0x00003240
#define RSX_PFIFO_CACHE1_DMA_GET                        0x00003244
#define RSX_PFIFO_CACHE1_REF_CNT                        0x00003248
#define RSX_PFIFO_CACHE1_DMA_SUBROUTINE                 0x0000324C
#define RSX_PFIFO_CACHE1_PULL0                          0x00003250 // TODO: Why doesn't the fields below exactly match LV1 ones?
#define     RSX_PFIFO_CACHE1_PULL0_HASH_FAILED          0x00000010
#define     RSX_PFIFO_CACHE1_PULL0_HASH_BUSY            0x00001000
#define     RSX_PFIFO_CACHE1_PULL0_BIT14  // [14:14], acquire: 0(idle),      1(busy)
#define     RSX_PFIFO_CACHE1_PULL0_BIT18  // [18:18], state:   0(idle),      1(busy)
#define     RSX_PFIFO_CACHE1_PULL0_BIT19  // [19:19],
#define     RSX_PFIFO_CACHE1_PULL0_BIT22  // [22:22], device:  0(s/w),       1(h/w)
#define     RSX_PFIFO_CACHE1_PULL0_BIT26  // [26:26], hash:    0(succeeded), 1(failed)
#define     RSX_PFIFO_CACHE1_PULL0_BIT30  // [30:30], access:  0(disable),   1(enable)
#define     RSX_PFIFO_CACHE1_PULL0_BIT31  // [31:31], fifo unset on init
#define RSX_PFIFO_CACHE1_PULL1                          0x00003254
#define RSX_PFIFO_CACHE1_HASH                           0x00003258
#define RSX_PFIFO_CACHE1_UNK325C                        0x0000325C
#define RSX_PFIFO_CACHE1_ACQUIRE_TIMEOUT                0x00003260
#define RSX_PFIFO_CACHE1_ACQUIRE_TIMESTAMP              0x00003264
#define RSX_PFIFO_CACHE1_ACQUIRE_VALUE                  0x00003268
#define RSX_PFIFO_CACHE1_SEMAPHORE                      0x0000326C
#define RSX_PFIFO_CACHE1_GET                            0x00003270
#define RSX_PFIFO_CACHE1_ENGINE                         0x00003280
#define RSX_PFIFO_CACHE1_DMA_DCOUNT                     0x000032A0
#define RSX_PFIFO_GRCTX_INSTANCE                        0x000032E0
#define RSX_PFIFO_UNK32E4                               0x000032E4
#define RSX_PFIFO_UNK32E8                               0x000032E8
#define RSX_PFIFO_UNK3304                               0x00003304

// PFB
#define RSX_PFB_BOOT_0                 (0x00100000)
#define     RSX_PFB_BOOT_0_RAM_AMOUNT                   0x00000003
#define     RSX_PFB_BOOT_0_RAM_AMOUNT_32MB              0x00000000
#define     RSX_PFB_BOOT_0_RAM_AMOUNT_4MB               0x00000001
#define     RSX_PFB_BOOT_0_RAM_AMOUNT_8MB               0x00000002
#define     RSX_PFB_BOOT_0_RAM_AMOUNT_16MB              0x00000003
#define     RSX_PFB_BOOT_0_RAM_WIDTH_128                0x00000004
#define     RSX_PFB_BOOT_0_RAM_TYPE                     0x00000028
#define     RSX_PFB_BOOT_0_RAM_TYPE_SGRAM_8MBIT         0x00000000
#define     RSX_PFB_BOOT_0_RAM_TYPE_SGRAM_16MBIT        0x00000008
#define     RSX_PFB_BOOT_0_RAM_TYPE_SGRAM_16MBIT_4BANK  0x00000010
#define     RSX_PFB_BOOT_0_RAM_TYPE_SDRAM_16MBIT        0x00000018
#define     RSX_PFB_BOOT_0_RAM_TYPE_SDRAM_64MBIT        0x00000020
#define     RSX_PFB_BOOT_0_RAM_TYPE_SDRAM_64MBITX16     0x00000028
#define     RSX_PFB_BOOT_0_UMA_ENABLE                   0x00000100
#define     RSX_PFB_BOOT_0_UMA_SIZE                     0x0000F000
#define RSX_PFB_DEBUG_0                (0x00100080)
#define     RSX_PFB_DEBUG_0_PAGE_MODE                   0x00000001
#define     RSX_PFB_DEBUG_0_REFRESH_OFF                 0x00000010
#define     RSX_PFB_DEBUG_0_REFRESH_COUNTX64            0x00003F00
#define     RSX_PFB_DEBUG_0_REFRESH_SLOW_CLK            0x00004000
#define     RSX_PFB_DEBUG_0_SAFE_MODE                   0x00008000
#define     RSX_PFB_DEBUG_0_ALOM_ENABLE                 0x00010000
#define     RSX_PFB_DEBUG_0_CASOE                       0x00100000
#define     RSX_PFB_DEBUG_0_CKE_INVERT                  0x10000000
#define     RSX_PFB_DEBUG_0_REFINC                      0x20000000
#define     RSX_PFB_DEBUG_0_SAVE_POWER_OFF              0x40000000
#define RSX_PFB_UNK0084                (0x00100084)
#define RSX_PFB_CFG0                   (0x00100200)
#define     RSX_PFB_CFG0_SCRAMBLE                       0x20000000
#define RSX_PFB_CFG1                   (0x00100204)
#define RSX_PFB_FIFO_DATA              (0x0010020C)
#define     RSX_PFB_FIFO_DATA_RAM_AMOUNT_MB_MASK        0xFFF00000
#define     RSX_PFB_FIFO_DATA_RAM_AMOUNT_MB_SHIFT               20
#define     RSX_PFB_FIFO_DATA_RAM_MODE_MASK             0x000FFFFF // DDR self refresh/refresh mode
#define RSX_PFB_UNK0320                (0x00100320)
#define RSX_PFB_UNK032C                (0x0010032C)  // Set by LV1 to 0xFFFF18FF
#define RSX_PFB_UNK0330                (0x00100330)  // Set by LV1 to 0xCCCC5044
#define RSX_PFB_UNK0334                (0x00100334)  // Set by LV1 to 0x33332022
#define RSX_PFB_UNK0338                (0x00100338)  // Set by LV1 to 0x0000000A
#define RSX_PFB_CLOSE_PAGE2            (0x0010033C)
#define RSX_PFB_UNK0340                (0x00100340)  // Set by LV1 to 0x000001FB
#define RSX_PFB_UNK0344                (0x00100344)  // Set by LV1 to 0x00000000
#define RSX_PFB_UNK0348                (0x00100348)  // Set by LV1 to 0x00000000
#define RSX_PFB_UNK0350                (0x00100350)  // Set by LV1 to 0x0000440A
#define RSX_PFB_UNK0354                (0x00100354)  // Set by LV1 to 0x00000806
#define RSX_PFB_TILE(i)                (0x00100600 + 16*(i))
#define RSX_PFB_TLIMIT(i)              (0x00100604 + 16*(i))
#define RSX_PFB_TSIZE(i)               (0x00100608 + 16*(i))
#define RSX_PFB_TSTATUS(i)             (0x0010060C + 16*(i))

// PGRAPH
#define RSX_PGRAPH_DEBUG_0             (0x00400080)
#define RSX_PGRAPH_INTR                (0x00400100)
#define RSX_PGRAPH_UNK00A4             (0x004000A4)
#define RSX_PGRAPH_NSOURCE             (0x00400108)
#define     RSX_PGRAPH_NSOURCE_NOTIFICATION            (1 <<  0)  // Notification pending
#define     RSX_PGRAPH_NSOURCE_DATA_ERROR              (1 <<  1)  // Data error pending
#define     RSX_PGRAPH_NSOURCE_PROTECTION_ERROR        (1 <<  2)  // Protection error
#define     RSX_PGRAPH_NSOURCE_RANGE_EXCEPTION         (1 <<  3)  // Range exception pending
#define     RSX_PGRAPH_NSOURCE_LIMIT_COLOR             (1 <<  4)  // Color limit erro pending
#define     RSX_PGRAPH_NSOURCE_LIMIT_ZETA              (1 <<  5)  // Zeta limit erro pending
#define     RSX_PGRAPH_NSOURCE_ILLEGAL_MTHD            (1 <<  6)  // Illegal method error pending
#define     RSX_PGRAPH_NSOURCE_DMA_R_PROTECTION        (1 <<  7)  // DMA read protection error pending
#define     RSX_PGRAPH_NSOURCE_DMA_W_PROTECTION        (1 <<  8)  // DMA write protection error pending
#define     RSX_PGRAPH_NSOURCE_FORMAT_EXCEPTION        (1 <<  9)  // Format exception error pending
#define     RSX_PGRAPH_NSOURCE_PATCH_EXCEPTION         (1 << 10)  // Patch exception error pending
#define     RSX_PGRAPH_NSOURCE_STATE_INVALID           (1 << 11)  // State invalid error pending
#define     RSX_PGRAPH_NSOURCE_DOUBLE_NOTIFY           (1 << 12)  // Double notify error pending
#define     RSX_PGRAPH_NSOURCE_NOTIFY_IN_USE           (1 << 13)  // Notify in use pending
#define     RSX_PGRAPH_NSOURCE_METHOD_CNT              (1 << 14)  // Method count error pending
#define     RSX_PGRAPH_NSOURCE_BFR_NOTIFICATION        (1 << 15)  // Bfr notification erro pending
#define     RSX_PGRAPH_NSOURCE_DMA_VTX_PROTECTION      (1 << 16)  // DMA vertex protection error pending
#define     RSX_PGRAPH_NSOURCE_BIT17                   (1 << 17)  // Idx inline reuse error pending
#define     RSX_PGRAPH_NSOURCE_BIT18                   (1 << 18)  // Invalid operation error pending
#define     RSX_PGRAPH_NSOURCE_BIT22                   (1 << 22)  // Texture A protection error pending
#define     RSX_PGRAPH_NSOURCE_BIT23                   (1 << 23)  // Texture V protection error pending
#define     RSX_PGRAPH_NSOURCE_BIT24                   (1 << 24)  // DMA Z-Cull protection error pending
#define     RSX_PGRAPH_NSOURCE_BIT25                   (1 << 25)  // SIP error pending
#define RSX_PGRAPH_CTX_CONTROL         (0x00400144)
#define RSX_PGRAPH_CTX_USER            (0x00400148)
#define RSX_PGRAPH_CTX_SWITCH(i)       (0x0040014C + 4*(i))
#define RSX_PGRAPH_CTXCTL_UNK0304      (0x00400304)
#define RSX_PGRAPH_CTXCTL_UCODE_STAT                    0x00400308
#define     RSX_PGRAPH_CTXCTL_UCODE_STAT_IP_MASK        0xFF000000
#define     RSX_PGRAPH_CTXCTL_UCODE_STAT_IP_SHIFT               24
#define     RSX_PGRAPH_CTXCTL_UCODE_STAT_OP_MASK        0x00FFFFFF
#define RSX_PGRAPH_CTXCTL_UNK0310      (0x00400310)
#define RSX_PGRAPH_CTXCTL_UCODE_INDEX  (0x00400324)
#define RSX_PGRAPH_CTXCTL_UCODE_DATA   (0x00400328)
#define RSX_PGRAPH_CTXCTL_CUR          (0x0040032c)
#define     RSX_PGRAPH_CTXCTL_CUR_LOADED                0x01000000
#define     RSX_PGRAPH_CTXCTL_CUR_INSTANCE              0x000FFFFF
#define RSX_PGRAPH_CTXCTL_NEXT         (0x00400330)
#define     RSX_PGRAPH_CTXCTL_NEXT_INSTANCE             0x000FFFFF
#define RSX_PGRAPH_DEBUG_2             (0x00400620)
#define RSX_PGRAPH_UNK0634             (0x00400634)
#define RSX_PGRAPH_TILE(i)             (0x00400D00 + 16*(i))
#define RSX_PGRAPH_DMA_A_XLATE_INST    (0x00401040)
#define RSX_PGRAPH_DMA_A_CONTROL       (0x00401044)
#define RSX_PGRAPH_DMA_A_LIMIT         (0x00401048)
#define RSX_PGRAPH_DMA_A_TLB_PTE       (0x0040104C)
#define RSX_PGRAPH_DMA_B_XLATE_INST    (0x00401080)
#define RSX_PGRAPH_DMA_B_CONTROL       (0x00401084)
#define RSX_PGRAPH_DMA_B_LIMIT         (0x00401088)
#define RSX_PGRAPH_DMA_B_TLB_PTE       (0x0040108C)
#define RSX_PGRAPH_UNK5000             (0x00405000)
