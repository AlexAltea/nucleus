/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"
#include "nucleus/cpu/cpu.h"

namespace cpu {

class HostCPU : public CPU {
    mem::Memory* memory;

public:
    HostCPU(mem::Memory* memory);

    // Thread management
    virtual void run() override;
    virtual void pause() override;
    virtual void stop() override;
};

}  // namespace cpu
