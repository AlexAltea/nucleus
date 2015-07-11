/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/rsx_pgraph.h"

#include <stack>
#include <thread>

namespace gpu {

struct rsx_device_t {
};

// LPAR DMA Control
struct rsx_dma_control_t {
    U8 unk[0x40];
    BE<U32> put; // CellGcmControl
    BE<U32> get; // CellGcmControl
    BE<U32> ref; // CellGcmControl
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

struct rsx_report_t {
    BE<U64> timestamp;
    BE<U32> value;
    BE<U32> padding;
};

struct rsx_reports_t {
    rsx_semaphore_t semaphore[0x100];
    U8 unk_notify[0x400];
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

union rsx_method_t
{
#define FIELD(from, to, type) struct{ U32:(32-to-1); type:(to-from+1); U32:from; }

    U32 method;

    FIELD( 1,  1, U32 flag_ni);        // Non-Incrementing method
    FIELD( 2,  2, U32 flag_jump);      // Jump command
    FIELD(14, 14, U32 flag_ret);       // Return command
    FIELD(30, 30, U32 flag_call);      // Call command
    FIELD( 3, 13, U32 method_count);   // Method arguments count
    FIELD(16, 29, U32 method_offset);  // Method offset (>> 2)
    FIELD( 3, 31, U32 jump_offset);    // Jump IO offset
    FIELD( 0, 29, U32 call_offset);    // Call IO offset (>> 2)

#undef FIELD
};

class RSX {
    // Execution engines
    PGRAPH pgraph;

    // Command processing engine: PFIFO
    U32 dma_semaphore;
    U32 dma_semaphore_offset;

    // Thread responsible of fetching methods and rendering
    std::thread* m_pfifo_thread;

    // Call stack
    std::stack<U32> m_pfifo_stack;

    // Connect to the global UI
    void connect();

public:
    // RSX Local Memory (mapped into the user space)
    rsx_device_t* device;
    rsx_dma_control_t* dma_control;
    rsx_driver_info_t* driver_info;
    rsx_reports_t* reports;

    // Display buffers stored as textures
    rsx_display_info_t display[8];
    U8 queued_display = 0;

    // IO Memory Access (mapped into GPU memory through FlexIO)
    std::vector<rsx_iomap_t> iomaps;

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

    // Initialization and method processing
    void init();

    void task();

    void method(U32 offset, U32 parameter);
};

}  // namespace gpu
