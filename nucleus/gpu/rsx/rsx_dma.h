/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Forward declarations
namespace mem { class Memory; }

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

    RSX_CONTEXT_DMA_FRAME_BUFFER            = 0xFEED0000,
    RSX_CONTEXT_DMA_HOST_BUFFER             = 0xFEED0001,
    RSX_CONTEXT_DMA_GSEMU_UNK0              = 0xFEED0003, // Related with SCEI's PlayStation 2 emulator
    RSX_CONTEXT_DMA_GSEMU_UNK1              = 0xFEED0004, // Related with SCEI's PlayStation 2 emulator
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

U08 dma_read8 (mem::Memory* memory, U32 dma_object, U32 offset);
U16 dma_read16(mem::Memory* memory, U32 dma_object, U32 offset);
U32 dma_read32(mem::Memory* memory, U32 dma_object, U32 offset);
U64 dma_read64(mem::Memory* memory, U32 dma_object, U32 offset);

void dma_write8 (mem::Memory* memory, U32 dma_object, U32 offset, U08 value);
void dma_write16(mem::Memory* memory, U32 dma_object, U32 offset, U16 value);
void dma_write32(mem::Memory* memory, U32 dma_object, U32 offset, U32 value);
void dma_write64(mem::Memory* memory, U32 dma_object, U32 offset, U64 value);

}  // namespace gpu
