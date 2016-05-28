/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/spu/spu_channel.h"

namespace cpu {
namespace frontend {
namespace spu {

enum MFCCommand {
    // MFC DMA Command flags
    MFC_BARRIER_ENABLE  = 0x0001,
    MFC_FENCE_ENABLE    = 0x0002,
    MFC_LIST_ENABLE     = 0x0004,  // SPU Only
    MFC_START_ENABLE    = 0x0008,  //  PU Only
    MFC_RESULT_ENABLE   = 0x0010,

    // MFC DMA Put Commands
    MFC_PUT_CMD         = 0x0020,
    MFC_PUTS_CMD        = 0x0028,  //  PU Only
    MFC_PUTR_CMD        = 0x0030,
    MFC_PUTF_CMD        = 0x0022,
    MFC_PUTB_CMD        = 0x0021,
    MFC_PUTFS_CMD       = 0x002A,  //  PU Only
    MFC_PUTBS_CMD       = 0x0029,  //  PU Only
    MFC_PUTRF_CMD       = 0x0032,
    MFC_PUTRB_CMD       = 0x0031,
    MFC_PUTL_CMD        = 0x0024,  // SPU Only
    MFC_PUTRL_CMD       = 0x0034,  // SPU Only
    MFC_PUTLF_CMD       = 0x0026,  // SPU Only
    MFC_PUTLB_CMD       = 0x0025,  // SPU Only
    MFC_PUTRLF_CMD      = 0x0036,  // SPU Only
    MFC_PUTRLB_CMD      = 0x0035,  // SPU Only

    // MFC DMA Get Commands
    MFC_GET_CMD         = 0x0040,
    MFC_GETS_CMD        = 0x0048,  //  PU Only
    MFC_GETF_CMD        = 0x0042,
    MFC_GETB_CMD        = 0x0041,
    MFC_GETFS_CMD       = 0x004A,  //  PU Only
    MFC_GETBS_CMD       = 0x0049,  //  PU Only
    MFC_GETL_CMD        = 0x0044,  // SPU Only
    MFC_GETLF_CMD       = 0x0046,  // SPU Only
    MFC_GETLB_CMD       = 0x0045,  // SPU Only

    // MFC Synchronization Commands
    MFC_SNDSIG_CMD      = 0x00A0,
    MFC_SNDSIGB_CMD     = 0x00A1,
    MFC_SNDSIGF_CMD     = 0x00A2,
    MFC_BARRIER_CMD     = 0x00C0,
    MFC_EIEIO_CMD       = 0x00C8,
    MFC_SYNC_CMD        = 0x00CC,

    // MFC Atomic Commands
    MFC_GETLLAR_CMD     = 0x00D0,  // SPU Only
    MFC_PUTLLC_CMD      = 0x00B4,  // SPU Only
    MFC_PUTLLUC_CMD     = 0x00B0,  // SPU Only
    MFC_PUTQLLUC_CMD    = 0x00B8,  // SPU Only
};

struct MFCListElement {
    BE<U16> s;     // Stall-and-notify bit (0x8000)
    BE<U16> lts;   // List element transfer size (LTS)
    BE<U32> leal;  // Low word of the 64-bit effective address
};

class MFC {
public:
    // Registers
    union {
        U64 ea;
        struct {
            U32 eah;
            U32 eal;
        };
    };
    union {
        U32 size_tag;
        struct {
            U16 tag;
            U16 size;
        };
    };
    U32 lsa;
};

class alignas(16) SPUState {
public:
    V128 r[128];  // General-Purpose Registers
    V128 s[128];  // Special-Purpose Registers

    // Program Counter
    U32 pc;

    // Memory Flow Controller
    MFC mfc;

    // Channels
    Channel<1, false> chTagStat;       // MFC Tag Group Status
    Channel<1, false> chListStallStat; // MFC List Stall-and-Notify Tag Acknowledgment
    Channel<1, false> chAtomicStat;    // MFC Atomic Command Status
    Channel<4,  true> chInMbox;        // SPU Inbound Mailbox
    Channel<1,  true> chOutMbox;       // SPU Outbound Mailbox
    Channel<1,  true> chOutIntrMbox;   // SPU Outbound Interrupt Mailbox
    Channel<1,  true> chSigNotify1;    // SPU Signal Notification Register 1
    Channel<1,  true> chSigNotify2;    // SPU Signal Notification Register 2
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
