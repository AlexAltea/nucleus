/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/rsx_pgraph.h"

#include <stack>
#include <thread>

// OpenGL dependencies
#include "nucleus/opengl.h"

struct rsx_device_t {
};

// LPAR DMA Control
struct rsx_dma_control_t {
    u8 unk[0x40];
    be_t<u32> put; // CellGcmControl
    be_t<u32> get; // CellGcmControl
    be_t<u32> ref; // CellGcmControl
    u32 unk2[2];
    be_t<u32> unk3; // TODO: Seems to be another GET / PUT pointer
};

// LPAR Driver Information
struct rsx_driver_info_t {
    be_t<u32> version_driver;
    be_t<u32> version_gpu;
    be_t<u32> memory_size;
    be_t<u32> hardware_channel;
    be_t<u32> nvcore_frequency;
    be_t<u32> memory_frequency;
    u8 unk1[0x10C0 - 6*4];
    struct head_t {
        be_t<u32> flip;
        be_t<u32> unk0[4];
        be_t<f32> unk_timestamp; // TEST: Is this a timestamp?
        be_t<u32> unk1[5];
        be_t<u32> unk_frameid; // TEST: Is this the number of frames?
        be_t<u32> unk2[4];
    } head[8]; // TEST: Only the first 2 are used?
    be_t<u32> handlers;
    be_t<u32> unk3;
    be_t<u32> unk4;
    be_t<u32> unk5;
    be_t<u32> handler_queue;
};

// LPAR Reports
struct rsx_semaphore_t {
    be_t<u32> value;
    be_t<u32> padding;
    be_t<u64> timestamp;
};

struct rsx_report_t {
    be_t<u64> timestamp;
    be_t<u32> value;
    be_t<u32> padding;
};

struct rsx_reports_t {
    rsx_semaphore_t semaphore[0x100];
    u8 unk_notify[0x400];
    rsx_report_t report[2048];
};

/**
 * Auxiliary classes
 */

// FlexIO mapped memory
struct rsx_iomap_t {
    u32 io;    // IO address (destination)
    u32 ea;    // EA space address (source)
    u32 size;  // Size of mapped region
};

// Display buffers (apparently not stored on RSX)
struct rsx_display_info_t {
    u32 offset;
    u32 pitch;
    u32 width;
    u32 height;
};

union rsx_method_t
{
#define FIELD(from, to, type) struct{ u32:(32-to-1); type:(to-from+1); u32:from; }

    u32 method;

    FIELD( 1,  1, u32 flag_ni);        // Non-Incrementing method
    FIELD( 2,  2, u32 flag_jump);      // Jump command
    FIELD(14, 14, u32 flag_ret);       // Return command
    FIELD(30, 30, u32 flag_call);      // Call command
    FIELD( 3, 13, u32 method_count);   // Method arguments count
    FIELD(16, 29, u32 method_offset);  // Method offset (>> 2)
    FIELD( 3, 31, u32 jump_offset);    // Jump IO offset
    FIELD( 0, 29, u32 call_offset);    // Call IO offset (>> 2)

#undef FIELD
};

class RSX
{
    // Rendering engine (Null, Software, OpenGL, Direct3D)
    PGRAPH* pgraph;

    // Command processing engine: PFIFO
    u32 dma_semaphore;
    u32 dma_semaphore_offset;

    // Thread responsible of fetching methods and rendering
    std::thread* m_pfifo_thread;

    // Call stack
    std::stack<u32> m_pfifo_stack;

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
    u8 queued_display = 0;

    // IO Memory Access (mapped into GPU memory through FlexIO)
    std::vector<rsx_iomap_t> iomaps;

    u32 io_read8(u32 offset);
    u32 io_read16(u32 offset);
    u32 io_read32(u32 offset);
    u32 io_read64(u64 offset);

    void io_write32(u8 offset, u8 value);
    void io_write32(u16 offset, u16 value);
    void io_write32(u32 offset, u32 value);
    void io_write32(u64 offset, u64 value);

    u32 get_ea(u32 io_addr);
    GLuint get_display();

    // Get current time in nanoseconds from PTIMER
    u64 ptimer_gettime();

    // Initialization and method processing
    void init();

    void task();

    void method(u32 offset, u32 parameter);
};
