/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <thread>

struct rsx_dma_control_t
{
    u8 unk[0x40];

    // CellGcmControl
    be_t<u32> put;
    be_t<u32> get;
    be_t<u32> ref;
};

struct rsx_report_t
{
    be_t<u64> timer;
	be_t<u32> value;
	be_t<u32> padding;
};

class RSX
{
    std::thread* m_thread_pfifo;

public:
    u32 regs[0x10000];

    // RSX Local Memory (mapped into the user space)
    rsx_dma_control_t* dma_control;
    rsx_report_t* reports;

    void init();
    void task();
};
