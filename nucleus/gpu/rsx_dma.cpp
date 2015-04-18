/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_dma.h"
#include "nucleus/emulator.h"

namespace gpu {

DMAObject dma_address(u32 dma_object)
{
    // NOTE: RAMIN is not emulated, therefore DMA Objects are hardcoded in this function
    switch (dma_object) {
    case RSX_CONTEXT_DMA_REPORT_LOCATION_LOCAL:
        return DMAObject{0x40300000, 0x8000, DMAObject::READWRITE}; // TODO: Inconsistency: Gitbrew says R starting at 0x1400, test says RW starting at 0x0.
    case RSX_CONTEXT_DMA_DEVICE_RW:
        return DMAObject{0x40000000, 0x1000, DMAObject::READWRITE};
    case RSX_CONTEXT_DMA_DEVICE_R:
        return DMAObject{0x40000000, 0x1000, DMAObject::READWRITE}; // TODO: Inconsistency: Gitbrew says R, test says RW
    case RSX_CONTEXT_DMA_SEMAPHORE_RW:
        return DMAObject{0x40100000, 0x1000, DMAObject::READWRITE};
    case RSX_CONTEXT_DMA_SEMAPHORE_R:
        return DMAObject{0x40100000, 0x1000, DMAObject::READWRITE}; // TODO: Inconsistency: Gitbrew says R, test says RW
    default:
        nucleus.log.warning(LOG_GPU, "Unknown DMA object (0x%08X)", dma_object);
        return DMAObject{};
    }
}

u8 dma_read8(u32 dma_object, u8 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory.read8(dma.addr + offset);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 8-bit read");
    return 0;
}

u16 dma_read16(u32 dma_object, u16 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory.read16(dma.addr + offset);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 16-bit read");
    return 0;
}

u32 dma_read32(u32 dma_object, u32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory.read32(dma.addr + offset);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 32-bit read");
    return 0;
}

u64 dma_read64(u32 dma_object, u64 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory.read64(dma.addr + offset);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 64-bit read");
    return 0;
}

void dma_write8(u32 dma_object, u32 offset, u8 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory.write8(dma.addr + offset, value);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write16(u32 dma_object, u32 offset, u16 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory.write16(dma.addr + offset, value);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write32(u32 dma_object, u32 offset, u32 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory.write32(dma.addr + offset, value);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write64(u32 dma_object, u32 offset, u64 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory.write64(dma.addr + offset, value);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA 64-bit write");
}

}  // namespace gpu
