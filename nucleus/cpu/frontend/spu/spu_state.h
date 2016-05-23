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

class alignas(16) SPUState {
public:
    V128 r[128];  // General-Purpose Registers
    V128 s[128];  // Special-Purpose Registers

    U32 pc;       // Program Counter
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
