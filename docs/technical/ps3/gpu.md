GPU Documentation
=================

The PlayStation 3 GPU is the *Reality Synthesizer* (**RSX**), designed by Sony and Nvidia. Its based on the NV47 chipset (Curie family). There is no official public documentation about the RSX from Sony. Nvidia doesn't provide low-level documentation about any of their GPUs either, making reverse engineering and intensive testing the only way to understand all aspects of this GPU. This document attempts to fill the gaps caused by the scarce information and to explain how Nucleus emulates the RSX. Some useful resources for this task were:

* PS3 Developer Wiki's [RSX](http://www.psdevwiki.com/ps3/RSX), [RSX FIFO Commands](http://www.psdevwiki.com/ps3/RSXFIFOCommands) and [Hypervisor Reverse Engineering](http://www.ps3devwiki.com/ps3/Hypervisor_Reverse_Engineering) pages.
* Reverse engineering of applications and system files (e.g. `libgcm_sys.sprx`, and RSX-related LV1/LV2 syscalls).
* Lots of tests in the [PS3 Autotests](https://github.com/AlexAltea/ps3autotests/tree/master/tests/gpu) repository.
* [Nouveau](http://nouveau.freedesktop.org/wiki/) open source drivers and information, specially [envytools](https://github.com/envytools/envytools/)

Accurantly emulating RSX results in heavy performance drops. Furthermore, these measures are not required for the vast majority of applications available for the PlayStation 3. For that matter, the following RSX emulation modes exist in Nucleus that will be referred to throughout this document:

* __Accurate Emulation__ (*AE*): Worse performance. Compatibility with any hypervisor, regardless of HLE or LLE.
* __Fast Emulation__ (*FE*): Good performance. Compatibility user-mode applications compiled with the SCEI PS3 SDK, running on top of CellOS-LV1 HLE or CellOS-LV2 HLE.

## Memory

The RSX exposes 3 Base Address Registers (BARs) detailed below:

| BAR    | Offset        | Size    | Description |
|--------|---------------|---------|-------------|
| *BAR0* | 0x28000000000 | 32 MB   | MMIO        |
| *BAR1* | 0x28080000000 | 256 MB  | VRAM        |
| *BAR2* | 0x28002000000 | *???*   | PRAMIN      |

Remarks:

* Note that the offset column provides information that concerns CellOS-LV1 exclusively.
* Nucleus with *AE* will emulate all BARs. Nucleus with *FE* will only superficially emulate BAR1.

## PRAMIN

TODO: Are the ranges below LV1-specific?

| Range               | Entries | Size     | Description    |
|---------------------|-------- |----------|----------------|
| `0x00000`-`0x0FFFF` |       1 |    64 KB | VBIOS (*?*)    |
| `0x40000`-`0x40FFF` |     256 | 16 bytes | DMA objects    |
| `0x50000`-`0x50FFF` |     128 | 32 bytes | Engine objects |

## Objects

### DMA Objects

TODO: Verify this.

| Class    | Name                      | Description |
|----------|---------------------------|-------------|
| `0x0002` | NV01_CONTEXT_DMA          | XDR to DDR  |
| `0x0003` | NV01_DEVICE               | DDR to XDR  |
| `0x003D` | NV01_MEMORY_LOCAL_BANKED  | DDR to DDR  |

### Engine Object

| Class    | Name                           |
|----------|--------------------------------|
| `0x0000` | NV01_NULL                      |
| `0x0039` | NV03_MEMORY_TO_MEMORY_FORMAT   |
| `0x3062` | NV30_CONTEXT_SURFACES_2D		|
| `0x309E` | NV30_CONTEXT_SURFACE_SWIZZLED	|
| `0x308A` | NV30_IMAGE_FROM_CPU			|
| `0x3089` | NV30_SCALED_IMAGE_FROM_MEMORY	|
| `0x4097` | NV40_CURIE_PRIMITIVE			|

## VRAM

Areas in VRAM

### RAMFC

Stores the PFIFO state of channels that are not active.

### RAMHT

Hash table of objects that a channel can use. These are 32-bit handles that can represent either DMA objects or Engine objects. (Individual objects can be shared between channels?).

A RAMHT entry is 8-bytes long:

| Bits | Description                               |
|------|-------------------------------------------|
| 32   | Object handle (e.g.: `0xCAFEBABE`)        |
| 9    | Channel ID                                |
| 3    | Engine ID                                 |
| 20   | Object offset (4 bits right-shifted)      |

## Engines

Following engines are available:

* __PFIFO__: Pulls commands from the command buffers and delivers them to other engines.
* __PGRAPH__: 2D and 3D rendering engine.

### PFIFO
Can control the following engines:

| ID | Name     |
|----|----------|
| 0  | SOFTWARE |
| 1  | PGRAPH   |

Note that *SOFTWARE* is a pseudo-engine that causes interrupts for every command. It's commonly used to execute driver functions in sync with other commands. In the PlayStation 3 it will cause the execution of the `lv1_gpu*` driver functions.

The PFIFO engine consists of following pieces:
* __Pusher__: Reads user commands from the buffer and pushes them to the cache.
* __Cache__: Queue holding the comands to be processed by the puller.
* __Puller__: Executes the commands or forwards them the corresponding engine.
* __Switcher__: Performs channel switches.

#### Channel

Consists of:

* Mode: Can be PIO or DMA (IB too?).
* PFIFO pusher state
* PFIFO cache state
* PFIFO puller state
* RAMFC/RAMHT contents?
* (Engine-specific state?)

#### Subchannel

There are 8 subchannels that can be bound to an object using the 0x0000 method offset. The GCM userland driver sets them to following values:

| ID | Handle       | ID     | Name             | Remarks                          |
|----|--------------|--------|------------------|----------------------------------|
| 0  | `0x31337000` | 0x4097 | *curie*          |                                  |
| 1  | `0x31337303` | 0x0039 | *m2mf*           | XDR to DDR.                      |
| 2  | `0x3137C0DE` | 0x0039 | *m2mf*           | DDR to XDR. Sometimes not bound. |
| 3  | `0x313371C3` | 0x3062 | *surf2d* (nv40)  |                                  |
| 4  | `0x31337A73` | 0x309E | *swzsurf* (nv40) |                                  |
| 5  | `0x31337808` | 0x308A | *ifc* (nv40)     |                                  |
| 6  | `0x3137AF00` | 0x3089 | *sifm* (nv40)    |                                  |
| 7  | `0xCAFEBABE` | *???*  | *sofware*        |                                  |

#### Methods

| Bits  | View (MSB:LSB)                        | Description                                 |
|-------|---------------------------------------|---------------------------------------------|
| 01:01 | `0X000000 00000000 00000000 00000000` |  Flag: Non-increment                        |
| 02:02 | `00X00000 00000000 00000000 00000000` |  Flag: Jump                                 |
| 14:14 | `00000000 000000X0 00000000 00000000` |  Flag: Return                               |
| 30:30 | `00000000 00000000 00000000 000000X0` |  Flag: Call                                 |
| 03:13 | `000XXXXX XXXXXX00 00000000 00000000` |  Method count                               |
| 16:29 | `00000000 00000000 AAABBBBB BBBBBB00` |  Method register (2 bits right-shifted) [1] |
| 03:31 | `000XXXXX XXXXXXXX XXXXXXXX XXXXXXXX` |  Jump offset                                |
| 00:29 | `XXXXXXXX XXXXXXXX XXXXXXXX XXXXXX00` |  Call offset (2 bits right-shifted)         |

Remarks:

1. The *method register* field consists of two fields: A 3-bit long *subchannel* identifier and a 11-bit long *offset* in that channel. At driver user-level, the whole 14-bit field can be considered as nothing but a 2-bit-rightshifted address where data goes in, optionally triggering an event. This notion is used on Nucleus for performance reasons.

Method offset:

* `0x0000`: Binds the specified object to the *subchannel* specified in the method header.
* `0x0004`-`0x00FC`: Methods executed by the PFIFO puller, rather than being passed to execution engines.
* `0x0180`-`0x01FC`: Methods passed to execution engines translating the hash parameter to an instance via RAMHT.

### PGRAPH

Also referred to as *Curie Graphics Engine*

#### Curie 3D Objects (NV4097)

TODO

## Devices

Shared by all RSX contexts.

| ID | Size   | Name    | Description    | Usage       |
|----|--------|---------|----------------|-------------|
|  0 | *???*  | *???*   | BAR0           | *???*       |
|  1 | *???*  | *???*   | Audio          | *???*       |
|  8 | 0x1000 | *???*   | Video RAM      | cellGcmInit |

## Contexts

*Cell OS Lv-1* supports a maximum of 3 RSX `SOFTWARE` class objects, which means only 3 RSX contexts can be created simultaneously, while the RSX is able to handle up to 8. Games and other user processes create one context. Nucleus only emulates a single process avoiding IPC's of any kind, therefore only 1 context is considered.

## Notifiers

TODO

## Reports

TODO

## Vertex Shaders

Vertex shaders are run in the *Vertex Processing Engine* (**VPE**). It reads 128-bit instructions from a 512 instruction buffer. Two PGRAPH registers point to the offset where new instructions are written (*LOAD*) and the offset where the execution of the shader starts (*START*) respectively.

### Registers

The vertex shaders can access following resources:

* 16 input registers `v[0]`-`v[15]` (*vector*)
* 16 output registers `o[0]`-`o[15]` (*vector*)
* 16 data registers `r[0]`-`r[15]` (*vector*)

#### Input/Output registers

| Input   | Description    |   | Output  | Description                             |
|---------|----------------|---|---------|-----------------------------------------|
| `v[0]`  | Position       |   | `o[0]`  | Position                                |
| `v[1]`  | Skin weights   |   | `o[1]`  | Back diffuse color                      |
| `v[2]`  | Normal         |   | `o[2]`  | Back specular color                     |
| `v[3]`  | Diffuse color  |   | `o[3]`  | Front diffuse color                     |
| `v[4]`  | Specular color |   | `o[4]`  | Front specular color                    |
| `v[5]`  | Fog            |   | `o[5]`  | Fog, Clip planes 0-2                    |
| `v[6]`  | Point size     |   | `o[6]`  | Point size, Clip planes 3-5 / Texture 9 |
| `v[7]`  | (*???*)        |   | `o[7]`  | Texture 0                               |
| `v[8]`  | Texture 0      |   | `o[8]`  | Texture 1                               |
| `v[9]`  | Texture 1      |   | `o[9]`  | Texture 2                               |
| `v[10]` | Texture 2      |   | `o[10]` | Texture 3                               |
| `v[11]` | Texture 3      |   | `o[11]` | Texture 4                               |
| `v[12]` | Texture 4      |   | `o[12]` | Texture 5                               |
| `v[13]` | Texture 5      |   | `o[13]` | Texture 6                               |
| `v[14]` | Texture 6      |   | `o[14]` | Texture 7                               |
| `v[15]` | Texture 7      |   | `o[15]` | Texture 8                               |

#### Constant registers:

TODO

#### Data registers:

TODO

### Opcodes

TODO

## Fragment Shaders

TODO

### Opcodes
| Name        | Opcode | Description |
|-------------|--------|-------------|
| `NOP`       |  0x00  | No-Operation |
| `MOV`       |  0x01  | Move |
| `MUL`       |  0x02  | Multiply |
| `ADD`       |  0x03  | Add |
| `MAD`       |  0x04  | Multiply-Add |
| `DP3`       |  0x05  | 3-component Dot Product |
| `DP4`       |  0x06  | 4-component Dot Product |
| `DST`       |  0x07  | Distance |
| `MIN`       |  0x08  | Minimum |
| `MAX`       |  0x09  | Maximum |
| `SLT`       |  0x0A  | Set-If-LessThan |
| `SGE`       |  0x0B  | Set-If-GreaterEqual |
| `SLE`       |  0x0C  | Set-If-LessEqual |
| `SGT`       |  0x0D  | Set-If-GreaterThan |
| `SNE`       |  0x0E  | Set-If-NotEqual |
| `SEQ`       |  0x0F  | Set-If-Equal |
| `FRC`       |  0x10  | Fraction (fract) |
| `FLR`       |  0x11  | Floor |
| `KIL`       |  0x12  | Kill fragment |
| `PK4`       |  0x13  | Pack four signed 8-bit values |
| `UP4`       |  0x14  | Unpack four signed 8-bit values |
| `DDX`       |  0x15  | Partial-derivative in x (Screen space derivative w.r.t. x) |
| `DDY`       |  0x16  | Partial-derivative in y (Screen space derivative w.r.t. y) |
| `TEX`       |  0x17  | Texture lookup |
| `TXP`       |  0x18  | Texture sample with projection (Projective texture lookup) |
| `TXD`       |  0x19  | Texture sample with partial differentiation (Texture lookup with derivatives) |
| `RCP`       |  0x1A  | Reciprocal |
| `RSQ`       |  0x1B  | Reciprocal Square Root |
| `EX2`       |  0x1C  | Exponentiation base 2 |
| `LG2`       |  0x1D  | Log base 2 |
| `LIT`       |  0x1E  | Lighting coefficients |
| `LRP`       |  0x1F  | Linear Interpolation |
| `STR`       |  0x20  | Set-If-True |
| `SFL`       |  0x21  | Set-If-False |
| `COS`       |  0x22  | Cosine |
| `SIN`       |  0x23  | Sine |
| `PK2`       |  0x24  | Pack two 16-bit floats |
| `UP2`       |  0x25  | Unpack two 16-bit floats |
| `POW`       |  0x26  | Power |
| `PKB`       |  0x27  | Pack bytes |
| `UPB`       |  0x28  | Unpack bytes |
| `PK16`      |  0x29  | Pack 16 bits |
| `UP16`      |  0x2A  | Unpack 16 |
| `BEM`       |  0x2B  | Bump-environment map (a.k.a. 2D coordinate transform) |
| `PKG`       |  0x2C  | Pack with sRGB transformation |
| `UPG`       |  0x2D  | Unpack gamma |
| `DP2A`      |  0x2E  | 2-component dot product with scalar addition |
| `TXL`       |  0x2F  | Texture sample with LOD |
| `TXB`       |  0x31  | Texture sample with bias |
| `TEXBEM`    |  0x33  | *???* |
| `TXPBEM`    |  0x34  | *???* |
| `BEMLUM`    |  0x35  | *???* |
| `REFL`      |  0x36  | Reflection vector |
| `TIMESWTEX` |  0x37  | *???* |
| `DP2`       |  0x38  | 2-component dot product |
| `NRM`       |  0x39  | Normalize |
| `DIV`       |  0x3A  | Division |
| `DIVSQ`     |  0x3B  | Divide by Square Root |
| `LIF`       |  0x3C  | Final part of LIT |
| `FENCT`     |  0x3D  | Fence T? |
| `FENCB`     |  0x3E  | Fence B? |
| `BRK`       |  0x40  | Break |
| `CAL`       |  0x41  | Subroutine call |
| `IFE`       |  0x42  | If |
| `LOOP`      |  0x43  | Loop |
| `REP`       |  0x44  | Repeat |
| `RET`       |  0x45  | Return |
    
TODO