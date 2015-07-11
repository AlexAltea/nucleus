/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gpu {

// RSX DMA class handles
enum {
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY0   = 0x66604200, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY1   = 0x66604201, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY2   = 0x66604202, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY3   = 0x66604203, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY4   = 0x66604204, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY5   = 0x66604205, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY6   = 0x66604206, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY7   = 0x66604207, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY8   = 0x66604208, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_NOTIFY_MAIN_0           = 0x6660420F,
    RSX_CONTEXT_DMA_SEMAPHORE_RW            = 0x66606660, // Target: lpar_reports[0x0000 : 0x1000] (Read/Write)
    RSX_CONTEXT_DMA_SEMAPHORE_R             = 0x66616661, // Target: lpar_reports[0x0000 : 0x1000] (Read)
    RSX_CONTEXT_DMA_REPORT_LOCATION_LOCAL   = 0x66626660, // Target: lpar_reports[0x1400 : 0x9400]
    RSX_CONTEXT_DMA_REPORT_LOCATION_MAIN    = 0xBAD68000,
    RSX_CONTEXT_DMA_DEVICE_RW               = 0x56616660,
    RSX_CONTEXT_DMA_DEVICE_R                = 0x56616661,
};

struct DMAObject {
    // Flags
    enum {
        READ      = 1 << 0,
        WRITE     = 1 << 1,
        READWRITE = READ | WRITE,
    };
    U32 addr;
    U32 size;
    U32 flags;
};

// RSX Direct Memory Access
DMAObject dma_address(U32 dma_object);

U8 dma_read8(U32 dma_object, U32 offset);
U16 dma_read16(U32 dma_object, U32 offset);
U32 dma_read32(U32 dma_object, U32 offset);
U64 dma_read64(U32 dma_object, U32 offset);

void dma_write8(U32 dma_object, U32 offset, U8 value);
void dma_write16(U32 dma_object, U32 offset, U16 value);
void dma_write32(U32 dma_object, U32 offset, U32 value);
void dma_write64(U32 dma_object, U32 offset, U64 value);

}  // namespace gpu
