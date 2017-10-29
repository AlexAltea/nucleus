/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "elf64.h"
#include "system.h"

#include <vector>

// Forward declarations
namespace mem { class Memory; }

namespace sys {

/**
 * ELF64 Loader
 * ============
 * Parses ELF64 files.
 * @tparam  E  Endianness
 */
template <template <typename> typename E>
class Elf64Loader {
protected:
    using Ehdr = Elf64_Ehdr<E>;
    using Phdr = Elf64_Phdr<E>;
    using Shdr = Elf64_Shdr<E>;

    // Segment loading
    void load_seg_load(System* sys, const Phdr& phdr, std::vector<Byte>& data)
    {
        U64 addr;

        if (!phdr.memsz) {
            break;
        }

        if (phdr.vaddr) {
            memory->allocFixed(phdr.vaddr, phdr.memsz);
            addr = phdr.vaddr;
        } else {
            addr = memory->alloc(phdr.memsz, phdr.align);
        }

        memory_h2g(addr, &elf[phdr.offset], phdr.filesz);
        if (phdr.flags & PF_X) {
            auto module = new cpu::frontend::ppu::Module(nucleus.cpu.get());
            module->parent = nucleus.cpu.get();
            module->address = phdr.vaddr;
            module->size = phdr.filesz;
            if (config.ppuTranslator & CPU_TRANSLATOR_MODULE) {
                module->analyze();
                module->recompile();
            }
            static_cast<cpu::Cell*>(nucleus.cpu.get())->ppu_modules.push_back(module);
        }
        break;
    }

    void load_seg_tls(System* sys, const Phdr& phdr, std::vector<Byte>& data)
    {
    }

    void process_seg_custom_os(System* sys, const Phdr& phdr, std::vector<Byte>& data)
    {
    }

    void process_seg_custom_proc(System* sys, const Phdr& phdr, std::vector<Byte>& data)
    {
    }

public:
    /**
     * Load the current ELF64 file into memory.
     */
    bool load(System* sys, std::vector<Byte>& data)
    {
        const Ehdr* ehdr = nullptr;
        const Phdr* phdr = nullptr;
        const Shdr* shdr = nullptr;
        Size ehdr_off = 0;
        Size phdr_off = 0;
        Size shdr_off = 0;

        if (data.size() < sizeof(Ehdr)) {
            return false;
        }

        ehdr = reinterpret_cast<Ehdr*>(&data[ehdr_off]);
        for (Size i = 0; i < ehdr.phnum; i++) {
            phdr_off = ehdr.phoff + i * sizeof(Phdr);
            phdr = reinterpret_cast<Phdr*>(&data[phdr_off]);

            Size phdr_type = phdr->type;
            switch (phdr_type) {
            case PT_LOAD:
                process_seg_load(sys, data);
                break;
            case PT_TLS:
                process_seg_tls(sys, data);
                break;
            }

            if (phdr_type >= PT_LOOS && phdr_type <= PT_HIOS) {
                process_seg_custom_os(sys, data);
            }
            if (phdr_type >= PT_LOPROC && phdr_type <= PT_HIPROC) {
                process_seg_custom_proc(sys, data);
            }
        }
    }
};

}  // namespace sys
