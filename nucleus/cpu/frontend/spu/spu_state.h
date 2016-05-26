/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {
namespace spu {

enum Channel {
    // SPU Channels
    SPU_RdEventStat      = 0x00,  // SPU Read Event Status
    SPU_WrEventMask      = 0x01,  // SPU Write Event Mask
    SPU_WrEventAck       = 0x02,  // SPU Write Event Acknowledgment
    SPU_RdSigNotify1     = 0x03,  // SPU Signal Notification 1
    SPU_RdSigNotify2     = 0x04,  // SPU Signal Notification 2
    SPU_WrDec            = 0x07,  // SPU Write Decrementer
    SPU_RdDec            = 0x08,  // SPU Read Decrementer
    SPU_RdEventMask      = 0x0B,  // SPU Read Event Mask
    SPU_RdMachStat       = 0x0D,  // SPU Read Machine Status
    SPU_WrSRR0           = 0x0E,  // SPU Write State Save-and-Restore
    SPU_RdSRR0           = 0x0F,  // SPU Read State Save-and-Restore
    SPU_WrOutMbox        = 0x1C,  // SPU Write Outbound Mailbox
    SPU_RdInMbox         = 0x1D,  // SPU Read Inbound Mailbox
    SPU_WrOutIntrMbox    = 0x1E,  // SPU Write Outbound Interrupt Mailbox

    // MFC Channels
    MFC_WrMSSyncReq      = 0x09,  // MFC Write Multisource Synchronization Request
    MFC_RdTagMask        = 0x0C,  // MFC Read Tag Group Query Mask Channe
    MFC_LSA              = 0x10,  // MFC LS Address
    MFC_EAH              = 0x11,  // MFC Effective Address High
    MFC_EAL              = 0x12,  // MFC Effective Address Low (or List Address)
    MFC_Size             = 0x13,  // MFC Transfer Size (or List Size)
    MFC_TagID            = 0x14,  // MFC Tag ID
    MFC_Cmd              = 0x15,  // MFC Command Opcode | MFC Class ID
    MFC_WrTagMask        = 0x16,  // MFC Write Tag Group Query Mask
    MFC_WrTagUpdate      = 0x17,  // MFC Write Tag Status Update Request
    MFC_RdTagStat        = 0x18,  // MFC Read Tag Group Status
    MFC_RdListStallStat  = 0x19,  // MFC Read List Stall-and-Notify Tag Status
    MFC_WrListStallAck   = 0x1A,  // MFC Write List Stall-and-Notify Tag Acknowledgment
    MFC_RdAtomicStat     = 0x1B,  // MFC Read Atomic Command Status
};

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

    U32 pc;       // Program Counter

    MFC mfc;      // Memory Flow Controller
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
