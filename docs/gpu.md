# GPU Documentation

The PlayStation 3 GPU is the *Reality Synthesizer* (**RSX**), designed by Sony and Nvidia. Its based on the NV47 chipset (Curie family). There is no official public documentation about the RSX from Sony. Nvidia doesn't provide low-level documentation about any of their GPUs either, making reverse engineering and intensive testing the only way to understand all aspects of this GPU. This document attempts to fill the gaps caused by the scarce information and to explain how Nucleus emulates the RSX. Some useful resources for this task were:

* PS3 Developer Wiki's [RSX](http://www.psdevwiki.com/ps3/RSX), [RSX FIFO Commands](http://www.psdevwiki.com/ps3/RSXFIFOCommands) and [Hypervisor Reverse Engineering](http://www.ps3devwiki.com/ps3/Hypervisor_Reverse_Engineering) pages.
* Reverse engineering of applications and system files (e.g. `libgcm_sys.sprx`, and RSX-related LV1/LV2 syscalls).
* Lots of tests in the [PS3 Autotests](https://github.com/AlexAltea/ps3autotests/tree/master/tests/gpu) repository.
* [Nouveau](http://nouveau.freedesktop.org/wiki/) open source drivers and information, specially [envytools](https://github.com/envytools/envytools/)

## Notifiers

TODO

## Reports

TODO

## Vertex Shaders

Vertex shaders are run in the *Vertex Processing Engine* (**VPE**). It reads 128-bit instructions from a 512 instruction buffer. Two PGRAPH registers point to the offset where new instructions are written (*LOAD*) and the offset where the execution of the shader starts (*START*) respectively.

### Registers:

The vertex shaders can access following resources:

#### Input registers:
| Register | Description    |
|----------|----------------|
| v[0]     | Position       |
| v[1]     | Skin weights   |
| v[2]     | Normal         |
| v[3]     | Diffuse color  |
| v[4]     | Diffuse color  |
| v[5]     | Fog            |
| v[6]     | Point size     |
| v[7]     | (*???*)        |
| v[8]     | Texture 0      |
| v[9]     | Texture 1      |
| v[10]    | Texture 2      |
| v[11]    | Texture 3      |
| v[12]    | Texture 4      |
| v[13]    | Texture 5      |
| v[14]    | Texture 6      |
| v[15]    | Texture 7      |

#### Output registers:

TODO

#### Constant registers:

TODO

#### Data registers:

TODO

### Opcodes

TODO

## Fragment Shaders

TODO

### Opcodes
| Name | Opcode | Description |
|------|--------|-------------|
| NOP | 0x00 | No-Operation |
| MOV | 0x01 | Move |
| MUL | 0x02 | Multiply |
| ADD | 0x03 | Add |
| MAD | 0x04 | Multiply-Add |
| DP3 | 0x05 | 3-component Dot Product |
| DP4 | 0x06 | 4-component Dot Product |
| DST | 0x07 | Distance |

TODO

## Curie Graphics Engine

TODO

### Curie 3D Objects (NV4097)

TODO