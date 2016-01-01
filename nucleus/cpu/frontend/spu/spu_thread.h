/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"

namespace cpu {
namespace frontend {
namespace spu {

// Forward declarations
class SPUState;

class SPUThread : public Thread {
public:
    std::unique_ptr<SPUState> state;

    SPUThread(CPU* parent = nullptr);
    ~SPUThread();

    virtual void start() override;
    virtual void task() override;

    // Control the thread once it is started
    virtual void run() override;
    virtual void pause() override;
    virtual void stop() override;
};

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
