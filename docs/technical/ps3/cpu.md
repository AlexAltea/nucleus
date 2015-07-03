CPU Documentation
=================

This document briefly describes the PlayStation 3 CPU, provides relevant links to specifications, ABIs and other official documentation and explains how Nucleus emulates it.

## Hardware
The PlayStation 3 is based on the *Cell Broadband Engine Architecture* (**Cell**). This processor combines 1 *PowerPC Processing Element* (PPE) and 8 *Synergistic Processing Elements* (SPEs). In user-mode only 6 SPEs are available since one is disabled and other is reserved for the system. A brief description about these elements as well as links to the official public documentation are provided in this document.

* [Cell Broadband Engine Architecture (Version 1.02 / October 11, 2007)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/1AEEE1270EA2776387257060006E61BA/$file/CBEA_v1.02_11Oct2007_pub.pdf)
* [Cell Broadband Engine Programming Handbook (Version 1.12 / April 3, 2009)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/7A77CCDF14FE70D5852575CA0074E8ED/$file/CellBE_Handbook_v1.12_3Apr09_pub.pdf)
* [Cell Broadband Engine Registers (Version 1.5 / April 2, 2007)](http://cell.scei.co.jp/pdf/CBE_Public_Registers_v15.pdf)

Other resources worth mentioning are:

* [IBM Full-System Simulator for the Cell Broadband Engine (Version 3.1 / 28 May 2009)](http://www.ibm.com/developerworks/)
* [Cell Broadband Engine SDK (Version 3.1)](http://www.ps3devwiki.com/ps3/Cell_Programming_IBM)

### PowerPC Processing Element (PPE)
The *PowerPC Processing Element* is a big-endian 64-bit dual threaded PowerPC processor. It consists of the *PowerPC Processor Unit* (PPU) and the *Power Processor Storage Subsystem* (PPSS). Nucleus refers to the whole core just as `PPU` in the source code. The supervisor, i.e. the LV2 kernel, uses 64-bit memory addresses. However, all applications and games are constrained to a 32-bit addressing environment (user-mode). Nucleus only attempts to emulate user-mode code, everything above is HLE, thus all addresses are treated as 32-bit long. Further information about the PowerPC64 architecture below:

* [PowerPC Microprocessor Family: The Programming Environments Manual for 64-bit Microprocessors (Version 3.0 / July 15, 2005)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/F7E732FF811F783187256FDD004D3797/$file/pem_64bit_v3.0.2005jul15.pdf)
* [PowerPC Microprocessor Family: Vector/SIMD Multimedia Extension Technology Programming Environments Manual (Version 2.07c / October 26, 2006)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/C40E4C6133B31EE8872570B500791108/$file/vector_simd_pem_v_2.07c_26Oct2006_cell.pdf)

### Synergistic Processing Element (SPE)
The *Synergistic Processing Elements* are big-endian 128-bit SIMD processor running a specially developed ISA. Each of them consists of a *Synergistic Processing Unit* (SPU), a *Memory Flow Controller* (MFC) and a *Local Storage* (LS). Nucleus refers to these each core just as `SPU` in the source code. Further information about the SPEs below:

* [SPU Application Binary Interface Specification (Version 1.9 / July 18, 2008)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/02E544E65760B0BF87257060006F8F20/$file/SPU_ABI-Specification_1.9.pdf)
* [SPU Assembly Language Specification (Version 1.7 / July 18, 2008)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/EFA2B196893B550787257060006FC9FB/$file/SPU_Assembly_Language_Specification_1.7.pdf)
* [SPU Instruction Set Architecture (Version 1.2 / January 27, 2007)](http://cell.scei.co.jp/pdf/SPU_ISA_v12.pdf)


## Analyzer
The analyzer in Nucleus is comprised of 3 stages over each loaded executable segment: Function detection, Control Flow Graph generation, Type detection. This information is required by the recompiler and can be optionally saved as a metadata file next to original ELF/PRX binary.

### Function detection
The whole segment is sequentially scanned generating following sets of addresses (a.k.a. *labels*):

* __Blocks__: If the preceding instruction is a branching instruction or an invalid PPU instruction, and it ends with another branching instruction, holding no invalid instructions in between. Obviously this doesn't represent the whole set of basic blocks, for which further block splitting is required.
* __Jumps__: The there is a known jump targeting (directly or indirectly) this address.
* __Calls__: The there is a known call targeting this address.

Finally, the set of labels containing functions is computed as:

> Functions := ((Blocks \ Jumps) U Calls)

Labels lying outside the segment range are removed.

### Control Flow Graph generation
Every function from the previous stage is scanned to build a graph of its basic blocks, the Control Flow Graph (*CFG* for short). A queue of labels represents the *blocks* to be scanned. Each block is scanned sequentially, branches add new targets to the queue. If a specific label from the queue is inside the current CFG, it's either discarded or the container block split. Once the queue is empty the algorithm finishes. Any branch outside the segment will discard the function.

### Type detection
The arguments/return type of each function is detected by monitoring reads/writes on the input/output registers of the PPU ABI. To avoid exponential complexities caused by path bruteforcing due to conditional branches, following assumptions are made:

* __Input parameters__: Every input register is originally read before the first conditional jump is encountered. Originally read means: read before any write occurs.
* __Output parameters__: Every output register is written in the leaves of the CFG (blocks ending with `blr`).


## Recompiler
The recompiler in Nucleus requires to run the analyzer in the first place. Any `LOAD` segment in ELF/PRX files with PPU-executable flags is recompiled Ahead of Time (AOT) with LLVM to a so called *Module*. Usually, this means every ELF/PRX binary will generate one Module in the form of an ELF object that is dynamically linked and optionally saved to be reused later on. The analyzer information is still required even when reusing the recompiler-emitted code.

### Control Flow Graph modifications
Following modifications are done to the original CFG constructed in the analyzer:

* __Prologue block__ or `prolog`: First basic block in every function. Responsible of:
    - Moving function arguments to registers allocated as local variables.
    - Getting the start-timestamp to profile the function (optional).
    - Other logging/debugging purposes.
* __Epilogue block__ or `epilog`: Last basic block in every function. Responsible of:
    - Returning the value stored in the corresponding output register.
    - Getting the end-timestamp to profile the function (optional).
* __Conditional branches__: In the case of the conditional branch instructions (except for conditional known jumps) we require to add extra basic blocks to implement the idea of *conditional call* or *conditional return* respectively.

In addition, the CFG might be further altered due to LLVM optimization passes.

### Registers
TODO

### Branching
The PPU branching instructions are classified according to following diagram:

|        | Known                      | Unknown           |
|--------|----------------------------|-------------------|
| Jump   | `b`, `ba`, `bc`, `bca`     | `bctr`, `bcctr`   |
| Call   | `bl`, `bla`, `bcl`, `bcla` | `bctrl`, `bcctrl` |
| Return | `blr`, `bclr`              |                   |

In *known branches* the target address can be computed ahead of time. In *unknown branches* it's not possible, so we call the register-specified function manually. These categories are implemented in the following way:

* __Known jumps__: Branches between basic blocks, which should be part of the same CFG.
* __Known calls__: Calls to functions whose arguments are the input registers specified by PPU ABI. These function arguments are moved to the register local variables in the prolog block. The rest of registers (e.g. r0, r1, r2), are extracted as usual from the global thread-local PPU state object.
* __Unknown jumps__: Transformed into a `bctrl` followed by a branch to the epilog.
* __Unknown calls__: Calls to the functions specified by CTR. It's assumed that every possible value of CTR at this point has been identified as a function by the analyzer.
* __Return__: Branch to the function's epilog block which always ends on a return instruction.

In addition, conditional jumps/calls can be easily implemented with LLVM's conditional branch instruction. Note that this system does not take into account the `blrl`, `bclrl` instructions for which no good approach has been designed yet.
