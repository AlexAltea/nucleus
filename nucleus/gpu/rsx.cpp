/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx.h"
#include "nucleus/emulator.h"

void RSX::init() {
    // HACK: We store the data in memory (the PS3 stores the data in the GPU and maps it later through a LV2 syscall)
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60100000, 0x1000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60200000, 0x4000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60300000, 0x10000);

    dma_control = nucleus.memory.ptr<rsx_dma_control_t>(0x60100000);
    driver_info = nucleus.memory.ptr<rsx_driver_info_t>(0x60200000);
    reports = nucleus.memory.ptr<rsx_report_t>(0x60300000);

    // Write driver information
    driver_info->version = 0x211;
    driver_info->coreFrequency = 500000000;
    driver_info->memoryFrequency = 650000000;

    // Prevent the FIFO from fetching commands
    dma_control->get = 0;
    dma_control->put = 0;

    m_pfifo_thread = new std::thread([&](){
        task();
    });
}

void RSX::task() {
    while (true) {
        // Wait until GET and PUT are different
        while (dma_control->get == dma_control->put) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        const u32 get = dma_control->get;
        const u32 put = dma_control->put;

        // Read next command
        const rsx_method_t cmd = { nucleus.memory.read32(io_address + get) };

        if (cmd.flag_jump) {
            dma_control->get = cmd.jump_offset << 2;
            continue;
        }
        if (cmd.flag_call) {
            m_pfifo_stack.push(get + 4);
            dma_control->get = cmd.call_offset << 2;
            continue;
        }
        if (cmd.flag_ret) {
            dma_control->get = m_pfifo_stack.top();
            m_pfifo_stack.pop();
            continue;
        }

        //nucleus.log.notice(LOG_GPU, "METHOD: 0x%X (+ %d args) @ IO:0x%X", cmd.method_offset << 2, cmd.method_count, get);
        for (int i = 0; i < cmd.method_count; i++) {
            const u32 argument = nucleus.memory.read32(io_address + get + 4*(i+1));
            regs[cmd.method_offset + i * cmd.flag_ni] = argument;
        }

        dma_control->get += 4 * (cmd.method_count + 1);
    }
}
