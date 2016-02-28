/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_dma.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/memory/memory.h"

namespace gpu {

DMAObject dma_address(U32 dma_object)
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
        logger.warning(LOG_GPU, "Unknown DMA object (0x%08X)", dma_object);
        return DMAObject{};
    }
}

U8 dma_read8(U32 dma_object, U32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory->read8(dma.addr + offset);
    }

    logger.warning(LOG_GPU, "Illegal DMA 8-bit read");
    return 0;
}

U16 dma_read16(U32 dma_object, U32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory->read16(dma.addr + offset);
    }

    logger.warning(LOG_GPU, "Illegal DMA 16-bit read");
    return 0;
}

U32 dma_read32(U32 dma_object, U32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory->read32(dma.addr + offset);
    }

    logger.warning(LOG_GPU, "Illegal DMA 32-bit read");
    return 0;
}

U64 dma_read64(U32 dma_object, U32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory->read64(dma.addr + offset);
    }

    logger.warning(LOG_GPU, "Illegal DMA 64-bit read");
    return 0;
}

void dma_write8(U32 dma_object, U32 offset, U8 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory->write8(dma.addr + offset, value);
    }

    logger.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write16(U32 dma_object, U32 offset, U16 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory->write16(dma.addr + offset, value);
    }

    logger.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write32(U32 dma_object, U32 offset, U32 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory->write32(dma.addr + offset, value);
    }

    logger.warning(LOG_GPU, "Illegal DMA 32-bit write");
}

void dma_write64(U32 dma_object, U32 offset, U64 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        return nucleus.memory->write64(dma.addr + offset, value);
    }

    logger.warning(LOG_GPU, "Illegal DMA 64-bit write");
}

}  // namespace gpu
