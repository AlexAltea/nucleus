/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"
#include "nucleus/gpu/gpu.h"
#include "nucleus/gpu/rsx/rsx_pgraph.h"

#include <stack>
#include <thread>

namespace gpu {
namespace rsx {

struct rsx_device_t {
};

// LPAR DMA Control
struct rsx_dma_control_t {
    U8 reserved[0x40];
    BE<U32> put;
    BE<U32> get;
    BE<U32> ref;
    U32 unk2[2];
    BE<U32> unk3; // TODO: Seems to be another GET / PUT pointer
};

// LPAR Driver Information
struct rsx_driver_info_t {
    BE<U32> version_driver;
    BE<U32> version_gpu;
    BE<U32> memory_size;
    BE<U32> hardware_channel;
    BE<U32> nvcore_frequency;
    BE<U32> memory_frequency;
    U8 unk1[0x10C0 - 6*4];
    struct head_t {
        BE<U32> flip;
        BE<U32> unk0[4];
        BE<F32> unk_timestamp; // TEST: Is this a timestamp?
        BE<U32> unk1[5];
        BE<U32> unk_frameid; // TEST: Is this the number of frames?
        BE<U32> unk2[4];
    } head[8]; // TEST: Only the first 2 are used?
    BE<U32> handlers;
    BE<U32> unk3;
    BE<U32> unk4;
    BE<U32> unk5;
    BE<U32> handler_queue;
};

// LPAR Reports
struct rsx_semaphore_t {
    BE<U32> value;
    BE<U32> padding;
    BE<U64> timestamp;
};

struct rsx_notify_t {
    BE<U64> timestamp;
    BE<U64> zero;
};

struct rsx_report_t {
    BE<U64> timestamp;
    BE<U32> value;
    BE<U32> padding;
};

struct rsx_reports_t {
    rsx_semaphore_t semaphore[0x100];
    rsx_notify_t notify[64]; // NOTE: Only the 8 first entries appear to be used
    rsx_report_t report[2048];
};

/**
 * Auxiliary classes
 */

// FlexIO mapped memory
struct rsx_iomap_t {
    U32 io;    // IO address (destination)
    U32 ea;    // EA space address (source)
    U32 size;  // Size of mapped region
};

// Display buffers (apparently not stored on RSX)
struct rsx_display_info_t {
    U32 offset;
    U32 pitch;
    U32 width;
    U32 height;
};

class RSX : public GPU {
    // Execution engines
    PGRAPH pgraph;

    // Command processing engine: PFIFO
    U32 dma_semaphore;
    U32 dma_semaphore_offset;

    // Thread responsible of fetching methods and rendering
    std::thread* m_pfifo_thread;

    // Call stack
    std::stack<U32> m_pfifo_stack;

public:
    // RSX Local Memory (mapped into the user space)
    rsx_device_t* device;
    rsx_dma_control_t* dma_control;
    rsx_driver_info_t* driver_info;
    rsx_reports_t* reports;

    // Display buffers stored as textures
    rsx_display_info_t display[8];
    U8 queued_display = 0;

    std::shared_ptr<mem::Memory> memory;

    // IO Memory Access (mapped into GPU memory through FlexIO)
    std::vector<rsx_iomap_t> iomaps;

    // Constructor
    RSX(std::shared_ptr<mem::Memory> memory, std::shared_ptr<gfx::IBackend> graphics);

    U32 io_read8(U32 offset);
    U32 io_read16(U32 offset);
    U32 io_read32(U32 offset);
    U32 io_read64(U64 offset);

    void io_write32(U8 offset, U8 value);
    void io_write32(U16 offset, U16 value);
    void io_write32(U32 offset, U32 value);
    void io_write32(U64 offset, U64 value);

    U32 get_ea(U32 io_addr);
    //GLuint get_display();

    // Get current time in nanoseconds from PTIMER
    U64 ptimer_gettime();

    void task();

    void method(U32 offset, U32 parameter);

    virtual gfx::Texture* getFrontBuffer() override;
};

}  // namespace rsx
}  // namespace gpu
