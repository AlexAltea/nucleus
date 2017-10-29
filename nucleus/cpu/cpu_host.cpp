/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cpu_host.h"

namespace cpu {

HostCPU::HostCPU(Emulator* emulator, mem::Memory* memory)
    : CPU(emulator, memory)
{
}

void HostCPU::run()
{
}

void HostCPU::pause()
{
}

void HostCPU::stop()
{
}

}  // namespace cpu
