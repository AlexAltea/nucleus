/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

struct rsx_report_t
{

};

class RSX
{

public:

    u32 regs[0x10000];

    // RSX Local Memory (mapped into the user space)
    rsx_report_t* local_reports; 
};
