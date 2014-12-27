/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/gpu/rsx_pgraph.h"

#include <stack>
#include <thread>

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
    struct unk2 {
        be_t<u32> flip;
        u8 unk[0x3C];
    } unk2[2]; // TODO: Find the name and exact number of these
};

// LPAR Reports
struct rsx_report_t {
    be_t<u64> timer;
    be_t<u32> value;
    be_t<u32> padding;
};

struct rsx_reports_t {
    u8 unk_semaphore[0x1000];
    u8 unk_notify[0x400];
    rsx_report_t report[2048];
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

struct DMAObject {
    // Flags
    enum {
        READ      = 1 << 0,
        WRITE     = 1 << 1,
        READWRITE = READ | WRITE,
    };
    u32 addr;
    u32 size;
    u32 flags;
};

class RSX
{
    // Rendering engine (Null, Software, OpenGL, DirectX)
    PGRAPH* pgraph;

    // Command processing engine: PFIFO
    u32 dma_semaphore;
    u32 dma_semaphore_offset;

    // Thread responsible of fetching methods and rendering
    std::thread* m_pfifo_thread;

    // Call stack
    std::stack<u32> m_pfifo_stack;

public:
    // RSX Local Memory (mapped into the user space)
    rsx_device_t* device;
    rsx_dma_control_t* dma_control;
    rsx_driver_info_t* driver_info;
    rsx_reports_t* reports;

    // IO Address (mapped into GPU memory through FlexIO)
    u32 io_address;

    void init();

    void task();

    void method(u32 offset, u32 parameter);

    // DMA Read-Write
    DMAObject dma_address(u32 dma_object);
    u32 dma_read32(u32 dma_object, u32 offset);
    void dma_write32(u32 dma_object, u32 offset, u32 value);
};
