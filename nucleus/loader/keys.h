/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

enum SelfKeyType {
    KEY_LV0   = 1,
    KEY_LV1   = 2,
    KEY_LV2   = 3,
    KEY_APP   = 4,
    KEY_ISO   = 5,
    KEY_LDR   = 6,
    KEY_UNK7  = 7,
    KEY_NPDRM = 8,
};

struct SelfKey {
    u32 self_type;
    u64 version;
    u16 revision;
    u32 curve_type;
    u8 erk[0x20];
    u8 riv[0x10];
    u8 pub[0x28];
    u8 priv[0x15];
};

static u8 NP_KLIC_FREE[0x10] = {
    0x72, 0xF9, 0x90, 0x78, 0x8F, 0x9C, 0xFF, 0x74, 0x57, 0x25, 0xF0, 0x8E, 0x4C, 0x12, 0x83, 0x87
};

static u8 NP_KLIC_KEY[0x10] = {
    0xF2, 0xFB, 0xCA, 0x7A, 0x75, 0xB0, 0x4E, 0xDC, 0x13, 0x90, 0x63, 0x8C, 0xCD, 0xFD, 0xD1, 0xEE
};

const SelfKey& getSelfKey(u32 type, u64 version, u16 revision);
