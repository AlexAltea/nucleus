# CPU Documentation

The PlayStation 3 is based on the *Cell Broadband Engine Architecture* (**Cell**). This processor combines 1 *PowerPC Processing Element* (PPE) and 8 *Synergistic Processing Elements* (SPEs). In user-mode only 6 SPEs are available since one is disabled and other is reserved for the system. A brief description about these elements as well as links to the official public documentation are provided in this document.

* [Cell Broadband Engine Architecture (Version 1.02 / October 11, 2007)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/1AEEE1270EA2776387257060006E61BA/$file/CBEA_v1.02_11Oct2007_pub.pdf)
* [Cell Broadband Engine Programming Handbook (Version 1.12 / April 3, 2009)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/7A77CCDF14FE70D5852575CA0074E8ED/$file/CellBE_Handbook_v1.12_3Apr09_pub.pdf)
* [Cell Broadband Engine Registers (Version 1.5 / April 2, 2007)](http://cell.scei.co.jp/pdf/CBE_Public_Registers_v15.pdf)

Other resources worth mentioning are:

* [IBM Full-System Simulator for the Cell Broadband Engine (Version 3.1 / 28 May 2009)](http://www.ibm.com/developerworks/)
* [Cell Broadband Engine SDK (Version 3.1)](http://www.ps3devwiki.com/ps3/Cell_Programming_IBM)

## PowerPC Processing Element (PPE)

The *PowerPC Processing Element* is a big-endian 64-bit dual threaded PowerPC processor. It consists of the *PowerPC Processor Unit* (PPU) and the *Power Processor Storage Subsystem* (PPSS). Nucleus refers to the whole core just as `PPU` in the source code. The supervisor, i.e. the LV2 kernel, uses 64-bit memory addresses. However, all applications and games are constrained to a 32-bit addressing environment (user-mode). Nucleus only attempts to emulate user-mode code, everything above is HLE, thus all addresses are treated as 32-bit long. Further information about the PowerPC64 architecture below:

* [PowerPC Microprocessor Family: The Programming Environments Manual for 64-bit Microprocessors (Version 3.0 / July 15, 2005)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/F7E732FF811F783187256FDD004D3797/$file/pem_64bit_v3.0.2005jul15.pdf)
* [PowerPC Microprocessor Family: Vector/SIMD Multimedia Extension Technology Programming Environments Manual (Version 2.07c / October 26, 2006)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/C40E4C6133B31EE8872570B500791108/$file/vector_simd_pem_v_2.07c_26Oct2006_cell.pdf)

## Synergistic Processing Element (SPE)

The *Synergistic Processing Elements* are big-endian 128-bit SIMD processor running a specially developed ISA. Each of them consists of a *Synergistic Processing Unit* (SPU), a *Memory Flow Controller* (MFC) and a *Local Storage* (LS). Nucleus refers to these each core just as `SPU` in the source code. Further information about the SPEs below:

* [SPU Application Binary Interface Specification (Version 1.9 / July 18, 2008)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/02E544E65760B0BF87257060006F8F20/$file/SPU_ABI-Specification_1.9.pdf)
* [SPU Assembly Language Specification (Version 1.7 / July 18, 2008)](https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/EFA2B196893B550787257060006FC9FB/$file/SPU_Assembly_Language_Specification_1.7.pdf)
* [SPU Instruction Set Architecture (Version 1.2 / January 27, 2007)](http://cell.scei.co.jp/pdf/SPU_ISA_v12.pdf)
