/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

namespace core {

/**
 * Host
 * ====
 * This class contains information about the host hardware and software.
 * Telemetry, logging and backend configuration depends on the information stored here.
 */
class Host {
    // Fill all information about the host machine
    void initialize();

    // Fill specific information about the host machine
    void initCPU();
    void initGPU();
    void initOS();

public:
    // CPU Information
    struct CPU {
        std::string name;
    };

    // GPU Information
    struct GPU {
        std::string name;
    };

    // OS Information
    struct OS {
        std::string name;
    };
    
    CPU cpu;
    OS os;
    Size ram;

    Host();
};

}  // namespace core
