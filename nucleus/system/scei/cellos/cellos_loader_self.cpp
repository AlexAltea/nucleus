/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cellos_loader_self.h"
#include "nucleus/common.h"
#include "nucleus/core/config.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/memory/guest_virtual/guest_virtual_memory.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/system/keys.h"
#include "nucleus/system/loader.h"
#include "nucleus/logger/logger.h"

#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/system/scei/cellos/lv2/sys_process.h"
#include "nucleus/system/scei/cellos/lv2/sys_prx.h"

#include "externals/aes.h"
#include <zlib.h>

#include <cstring>

namespace sys {
namespace scei {
namespace cellos {

bool SELFLoader::open(fs::File* file)
{
    const U64 selfSize = file->attributes().size;
    self.resize(selfSize);
    file->seek(0, fs::SeekSet);
    file->read(&self[0], selfSize);

    switch (detectFiletype(file)) {
    case FILETYPE_SELF:
        decrypt();
        return true;

    case FILETYPE_ELF:
        elf.resize(selfSize);
        memcpy(&elf[0], &self[0], selfSize);
        return true;

    default:
        return false;
    }
}

bool SELFLoader::open(LV2* lv2, const std::string& path)
{
    fs::File* file = lv2->vfs.openFile(path, fs::Read);
    if (!file) {
        return false;
    }

    auto result = open(file);
    delete file;
    return result;
}

bool SELFLoader::load_elf(LV2* lv2)
{
    if (elf.empty()) {
        return false;
    }
    auto* cpu = dynamic_cast<cpu::GuestCPU*>(lv2->getEmulator()->cpu.get());
    auto* mem = dynamic_cast<mem::GuestVirtualMemory*>(lv2->getEmulator()->memory.get());

    const auto& ehdr = (Ehdr&)elf[0];

    // Loading program header table
    for (U64 i = 0; i < ehdr.phnum; i++) {
        const auto& phdr = (Phdr&)elf[ehdr.phoff + i*sizeof(Phdr)];

        switch (phdr.type.ToLE()) {
        case PT_LOAD:
            if (!phdr.memsz) {
                break;
            }

            mem->getSegment(mem::SEG_MAIN_MEMORY).allocFixed(phdr.vaddr, phdr.memsz);
            mem->memcpy_h2g(phdr.vaddr, &elf[phdr.offset], phdr.filesz);
            if (phdr.flags & PF_X) {
                auto module = new cpu::frontend::ppu::Module(cpu);
                module->parent = cpu;
                module->address = phdr.vaddr;
                module->size = phdr.filesz;
                if (config.ppuTranslator & CPU_TRANSLATOR_MODULE) {
                    module->analyze();
                    module->recompile();
                }
                cpu->ppu_modules.push_back(module);
            }
            break;

        case PT_TLS:
            // TODO: ?
            break;


        case PT_PROC_PARAM:
            if (!phdr.filesz) {
                lv2->proc.param.sdk_version = 0xFFFFFFFF;
                lv2->proc.param.malloc_pagesize = 0x100000;
            }
            else {
                lv2->proc.param = (sys::sys_process_param_t&)elf[phdr.offset];
            }
            break;

        case PT_PRX_PARAM:
            if (!phdr.filesz) {
                logger.error(LOG_LOADER, "Invalid PRX_PARAM segment");
            }
            else {
                lv2->proc.prx_param = (sys::sys_process_prx_param_t&)elf[phdr.offset];
            }
            break;
        }
    }
    return true;
}

bool SELFLoader::load_prx(LV2* lv2, sys::sys_prx_t& prx)
{
    if (elf.empty()) {
        return false;
    }

    auto* cpu = dynamic_cast<cpu::GuestCPU*>(lv2->getEmulator()->cpu.get());
    auto* mem = dynamic_cast<mem::GuestVirtualMemory*>(lv2->getEmulator()->memory.get());

    const auto& ehdr = (Ehdr&)elf[0];
    U32 base_addr;

    // Loading program header table
    for (U64 i = 0; i < ehdr.phnum; i++) {
        const auto& phdr = (Phdr&)elf[ehdr.phoff + i*sizeof(Phdr)];

        if (phdr.type == PT_LOAD) {
            // Allocate memory and copy segment contents
            const U32 addr = mem->getSegment(mem::SEG_MAIN_MEMORY).alloc(phdr.memsz, 0x10000);
            mem->memcpy_h2g(addr, &elf[phdr.offset], phdr.filesz);

            // Add information for PRX Object
            sys::sys_prx_segment_t segment;
            segment.flags = (U32)phdr.flags;
            segment.align = (U32)phdr.align;
            segment.size_file = (U32)phdr.filesz;
            segment.size_memory = (U32)phdr.memsz;
            segment.initial_addr = (U32)phdr.vaddr;
            segment.prx_offset = (U32)phdr.offset;
            segment.addr = addr;
            prx.segments.push_back(segment);

            // Get FNID / addr pairs
            if (phdr.paddr != 0) {
                const auto& module = (sys::sys_prx_module_info_t&)elf[phdr.paddr];
                prx.name = module.name;
                prx.version = module.version;

                // Get FNID / addr pairs
                U32 offset = module.exports_start;
                while (offset < module.exports_end) {
                    const auto& library = (sys::sys_prx_library_info_t&)elf[phdr.offset + offset];
                    offset += library.size;

                    sys::sys_prx_library_t lib;
                    if (library.name_addr) {
                        lib.name = (char*)(&elf[phdr.offset + library.name_addr]);
                    }
                    for (U32 i = 0; i < library.num_func; i++) {
                        const U32 fnid = ((BE<U32>&)elf[phdr.offset + library.fnid_addr + 4*i]).ToLE();
                        const U32 stub = ((BE<U32>&)elf[phdr.offset + library.fstub_addr + 4*i]).ToLE();
                        lib.exports[fnid] = stub;
                    }
                    prx.exported_libs.push_back(lib);
                }

                // Patch pointers to other PRXs
                offset = module.imports_start;
                while (offset < module.imports_end) {
                    const auto& library = (sys::sys_prx_library_info_t&)elf[phdr.offset + offset];
                    offset += library.size;

                    sys::sys_prx_library_t lib;
                    if (library.name_addr) {
                        lib.name = (char*)(&elf[phdr.offset + library.name_addr]);
                    }
                    for (U32 i = 0; i < library.num_func; i++) {
                        const U32 fnid = ((BE<U32>&)elf[phdr.offset + library.fnid_addr + 4*i]).ToLE();
                        const U32 stub = phdr.offset + library.fstub_addr + 4*i;
                        lib.exports[fnid] = stub;
                    }
                    prx.imported_libs.push_back(lib);
                }
            }

            // Update offsets according to the base address where the PRX was allocated
            // TODO: Probably hardcoding i==1 isn't a good idea.
            if (i == 1) {
                // Update FNID / addr pairs
                for (auto& lib : prx.exported_libs) {
                    for (auto& stub : lib.exports) {
                        U32 value = stub.second;
                        for (const auto& segment : prx.segments) {
                            if (segment.initial_addr <= value && value < segment.initial_addr + segment.size_file) {
                                value = value + (segment.addr - segment.initial_addr);
                            }
                        }
                        stub.second = value;
                    }
                }
                // Update FNID / addr pairs
                for (auto& lib : prx.imported_libs) {
                    for (auto& stub : lib.exports) {
                        U32 value = stub.second;
                        for (const auto& segment : prx.segments) {
                            if (segment.prx_offset <= value && value < segment.initial_addr + segment.size_file) {
                                value = value + (segment.addr - segment.initial_addr - prx.segments[0].prx_offset);
                            }
                        }
                        stub.second = value;
                    }
                }
            }
        }

        if (phdr.type == PT_SCE_PPURELA) {
            for (U32 i = 0; i < phdr.filesz; i += sizeof(sys::sys_prx_relocation_info_t)) {
                const auto& rel = (sys::sys_prx_relocation_info_t&)elf[phdr.offset + i];

                // Address that needs to be patched
                U32 addr = prx.segments[rel.index_addr].addr + rel.offset;
                U32 value = 0;

                switch (rel.type.ToLE()) {
                case R_PPC64_ADDR32:
                    value = (U32)prx.segments[rel.index_value].addr + rel.ptr;
                    mem->write32(addr, value);
                    break;

                case R_PPC64_ADDR16_LO:
                    value = (U16)rel.ptr;
                    mem->write16(addr, value);
                    break;

                case R_PPC64_ADDR16_HI:
                    value = (U16)(prx.segments[rel.index_value].addr >> 16);
                    mem->write16(addr, value);
                    break;

                case R_PPC64_ADDR16_HA:
                    value = (U16)(prx.segments[1].addr >> 16);
                    mem->write16(addr, value);
                    break;

                default:
                    logger.error(LOG_LOADER, "Unknown relocation type!");
                    break;
                }
            }
        }
    }

    for (const auto& importedLib : prx.imported_libs) {

        // TODO: Following libraries are DECR only. Ignore them (TODO: Don't do this)
        if (importedLib.name == "cellLibprof") {
            continue;
        }

        const sys::sys_prx_library_t* targetLibrary = nullptr;

        // Find library (TODO: This is very inefficient)
        for (const auto& object : lv2->objects) {
            if (object.second->getType() == sys::SYS_PRX_OBJECT) {
                const auto* imported_prx = (sys::sys_prx_t*)object.second->getData();
                for (const auto& exportedLib : imported_prx->exported_libs) {
                    if (exportedLib.name == importedLib.name) {
                        targetLibrary = &exportedLib;
                        break;
                    }
                }
            }
        }

        if (!targetLibrary) {
            return false;
        }

        for (const auto& import : importedLib.exports) {
            const U32 fnid = import.first;
            mem->write32(import.second, targetLibrary->exports.at(fnid));
        }
    }

    // Recompile executable segments
    for (auto& prx_segment : prx.segments) {
        if (prx_segment.flags & PF_X) {
            auto segment = new cpu::frontend::ppu::Module(cpu);
            segment->address = prx_segment.addr;
            segment->size = prx_segment.size_file;
            if (config.ppuTranslator & CPU_TRANSLATOR_MODULE) {
                segment->analyze();
                segment->recompile();
            }
            cpu->ppu_modules.push_back(segment);
        }
    }
    return true;
}

void SELFLoader::close()
{
    elf.clear();
    self.clear();
}

bool SELFLoader::decryptMetadata()
{
    aes_context aes;
    const auto& sce_header = (SceHeader&)self[0x0];
    const auto& self_header = (SelfHeader&)self[0x20];
    const auto& app_info = (AppInfo&)self[0x70];
    const auto key = getSelfKey(app_info.self_type, app_info.version, sce_header.flags);

    auto& meta_info = (MetadataInfo&)self[sizeof(SceHeader) + sce_header.meta];

    // Decrypt NPDRM Layer if necessary
    U08 npdrm_key[0x10];
    U08 npdrm_iv[0x10] = {};
    U32 offset = self_header.controloff;
    while (offset < self_header.controloff + self_header.controlsize) {
        const auto& ctrl = (ControlInfo&)self[offset];
        offset += ctrl.size;
        if (ctrl.type != 3) {
            continue;
        }

        switch (ctrl.npdrm.license.ToLE()) {
        case 1: // Network license
        case 2: // Local license
            logger.error(LOG_LOADER, "NPDRM Network / Local licenses not yet supported");
            break;
        case 3: // Free license
            memcpy(npdrm_key, NP_KLIC_FREE, 0x10);
            break;
        }

        // Decrypt our key with NP_KLIC_KEY and use it to decrypt the NPDRM Layer of the Metadata Info
        aes_setkey_dec(&aes, NP_KLIC_KEY, 128);
        aes_crypt_ecb(&aes, AES_DECRYPT, npdrm_key, npdrm_key);
        aes_setkey_dec(&aes, npdrm_key, 128);
        aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(MetadataInfo), npdrm_iv, (U08*)&meta_info, (U08*)&meta_info);
        break;
    }

    // Decrypt Metadata Info
    U08 metadata_iv[0x10];
    memcpy(metadata_iv, key.riv, 0x10);
    aes_setkey_dec(&aes, key.erk, 256);
    aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(MetadataInfo), metadata_iv, (U08*)&meta_info, (U08*)&meta_info);

    // Decrypt Metadata Headers (Metadata Header + Metadata Section Headers)
    U08 ctr_stream_block[0x10];
    U08* meta_headers = (U08*)&self[sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo)];
    U32 meta_headers_size = sce_header.hsize - (sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo));
    Size ctr_nc_off = 0;
    aes_setkey_enc(&aes, meta_info.key, 128);
    aes_crypt_ctr(&aes, meta_headers_size, &ctr_nc_off, meta_info.iv, ctr_stream_block, meta_headers, meta_headers);

    return true;
}

U32 SELFLoader::getDecryptedElfSize()
{
    U32 size = 0;
    const auto& sce_header = (SceHeader&)self[0x0];
    const auto& self_header = (SelfHeader&)self[0x20];
    const auto& ehdr = (Ehdr&)self[self_header.elfoff];

    // Check the maximum offset referenced by the PHDRs in the Metadata Headers
    const U32 meta_header_off = sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo);
    const auto& meta_header = (MetadataHeader&)self[meta_header_off];
    for (U32 i = 0; i < meta_header.section_count; i++) {
        const auto& meta_shdr = (MetadataSectionHeader&)self[meta_header_off + sizeof(MetadataHeader) + i*sizeof(MetadataSectionHeader)];
        const auto& meta_phdr = (Phdr&)self[self_header.phdroff + meta_shdr.program_idx * sizeof(Phdr)];
        if (meta_shdr.type != 2) {
            continue;
        }
        if (meta_shdr.compressed == 2 && size < meta_phdr.offset + meta_phdr.filesz) {
            size = meta_phdr.offset + meta_phdr.filesz;
        }
        else if (size < meta_phdr.offset + meta_shdr.data_size) {
            size = meta_phdr.offset + meta_phdr.filesz;
        }
    }
    // Check the maximum offset referenced by the SHDRs in the EHDR Header
    if (size < ehdr.shoff + ehdr.shnum * sizeof(Shdr)) {
        size = ehdr.shoff + ehdr.shnum * sizeof(Shdr);
    }
    return size;
}

bool SELFLoader::decrypt()
{
    if (self.empty()) {
        return false;
    }

    const auto& sce_header = (SceHeader&)self[0x0];
    const auto& self_header = (SelfHeader&)self[0x20];

    /**
     * Debug SELF
     */
    if (sce_header.flags == 0x8000) {
        const U64 elfSize = self.size() - sce_header.hsize;
        elf.resize(elfSize);
        memcpy(&elf[0], &self[sce_header.hsize], elfSize);
    }

    /**
     * Retail SELF
     */
    else {
        // Get Metadata Information
        decryptMetadata();
        const U32 meta_header_off = sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo);
        const auto& meta_header = (MetadataHeader&)self[meta_header_off];
        const U08* data_keys = (U08*)&self[meta_header_off + sizeof(MetadataHeader) + meta_header.section_count * sizeof(MetadataSectionHeader)];

        // Get ELF size and allocate/initialize it
        const U64 elfSize = getDecryptedElfSize();
        elf.resize(elfSize);

        // Copy the EHDR, PHDRs and SHDRs of the ELF
        const auto& ehdr = (Ehdr&)self[self_header.elfoff];
        Byte* self_phdrs = &self[self_header.phdroff];
        Byte* self_shdrs = &self[self_header.shdroff];
        Byte* elf_phdrs = &elf[ehdr.phoff];
        Byte* elf_shdrs = &elf[ehdr.shoff];
        memcpy(&elf[0], &ehdr, sizeof(Ehdr));
        memcpy(elf_phdrs, self_phdrs, ehdr.phnum * sizeof(Phdr));
        memcpy(elf_shdrs, self_shdrs, ehdr.shnum * sizeof(Shdr));

        // Write Data
        aes_context aes;
        for (U32 i = 0; i < meta_header.section_count; i++) {
            const auto& meta_shdr = (MetadataSectionHeader&)self[meta_header_off + sizeof(MetadataHeader) + i*sizeof(MetadataSectionHeader)];
            const auto& meta_phdr = (Phdr&)self[self_header.phdroff + meta_shdr.program_idx * sizeof(Phdr)];
            // Check if PHDR type
            if (meta_shdr.type != 2) {
                continue;
            }

            U08* data_decrypted = new U08[meta_shdr.data_size.ToLE()]();
            U08* data_decompressed = new U08[meta_phdr.filesz.ToLE()]();

            // Decrypt if necessary
            if (meta_shdr.encrypted == 3) {
                U08 data_key[0x10];
                U08 data_iv[0x10];
                memcpy(data_key, data_keys + meta_shdr.key_idx * 0x10, 0x10);
                memcpy(data_iv, data_keys + meta_shdr.iv_idx * 0x10, 0x10);
                memcpy(data_decrypted, &self[meta_shdr.data_offset], meta_shdr.data_size);

                // Perform AES-CTR encryption on the data
                U08 ctr_stream_block[0x10] = {};
                Size ctr_nc_off = 0;
                aes_setkey_enc(&aes, data_key, 128);
                aes_crypt_ctr(&aes, meta_shdr.data_size, &ctr_nc_off, data_iv, ctr_stream_block, data_decrypted, data_decrypted);
            }

            // Decompress if necessary
            if (meta_shdr.compressed == 2) {
                unsigned long length = meta_phdr.filesz;
                uncompress(data_decompressed, &length, data_decrypted, (U32)meta_shdr.data_size);
                memcpy(&elf[meta_phdr.offset], data_decompressed, meta_phdr.filesz);
            }
            else {
                memcpy(&elf[meta_phdr.offset], data_decrypted, meta_shdr.data_size);
            }

            delete[] data_decrypted;
            delete[] data_decompressed;
        }
    }
    return true;
}

U16 SELFLoader::getMachine()
{
    if (elf.empty()) {
        return EM_NONE;
    }
    const auto& ehdr = (Ehdr&)elf[0];
    return ehdr.machine;
}

U64 SELFLoader::getEntry()
{
    if (elf.empty()) {
        return 0;
    }
    const auto& ehdr = (Ehdr&)elf[0];
    return ehdr.entry;
}

void SELFLoader::process_seg_custom_os(System* sys, const Phdr& phdr, std::vector<Byte>& data)
{
    auto* lv2 = dynamic_cast<LV2*>(sys);

    if (phdr.type == PT_PROC_PARAM) {
        if (!phdr.filesz) {
            lv2->proc.param.sdk_version = 0xFFFFFFFF;
            lv2->proc.param.malloc_pagesize = 0x100000;
        }
        else {
            lv2->proc.param = (sys_process_param_t&)elf[phdr.offset];
        }
    }
    if (phdr.type == PT_PRX_PARAM) {
        if (!phdr.filesz) {
            logger.error(LOG_LOADER, "Invalid PRX_PARAM segment");
        }
        else {
            lv2->proc.prx_param = (sys_process_prx_param_t&)elf[phdr.offset];
        }
    }
}

void SELFLoader::process_seg_custom_proc(System* sys, const Phdr& phdr, std::vector<Byte>& data)
{

}

}  // namespace cellos
}  // namespace scei
}  // namespace sys
