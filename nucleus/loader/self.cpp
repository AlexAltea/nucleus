/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "self.h"
#include "nucleus/common.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/local_file.h"
#include "nucleus/syscalls/lv2/sys_process.h"
#include "nucleus/syscalls/lv2/sys_prx.h"
#include "externals/aes.h"
#include "externals/zlib/zlib.h"
#include "keys.h"
#include "loader.h"

#include <cstring>
#include <vector>

SELFLoader::SELFLoader()
{
    m_elf = nullptr;
    m_self = nullptr;
    m_elf_size = 0;
    m_self_size = 0;
}

SELFLoader::SELFLoader(const std::string& path)
{
    open(path);
}

SELFLoader::~SELFLoader()
{
    close();
}

bool SELFLoader::open(const std::string& path)
{
    vfsLocalFile file;
    if (!file.Open(path, vfsRead)) {
        return false;
    }

    m_self_size = file.GetSize();
    m_self = new char[m_self_size];
    file.Seek(0, vfsSeekSet);
    file.Read(m_self, m_self_size);

    if (detectFiletype(path) == FILETYPE_SELF) {
        decrypt();
        return true;
    }
    else if (detectFiletype(path) == FILETYPE_ELF) {
        m_elf_size = m_self_size;
        m_elf = new char[m_elf_size];
        memcpy(m_elf, m_self, m_elf_size);
        return true;
    }
    
    return false;
}

bool SELFLoader::load_elf()
{
    if (!m_elf) {
        return false;
    }

    const auto& ehdr = (Elf64_Ehdr&)m_elf[0];

    // Loading program header table
    for (u64 i = 0; i < ehdr.phnum; i++) {
        const auto& phdr = (Elf64_Phdr&)m_elf[ehdr.phoff + i*sizeof(Elf64_Phdr)];

        switch (phdr.type.ToLE()) {
        case PT_LOAD:
            if (!phdr.memsz) {
                break;
            }

            nucleus.memory(SEG_MAIN_MEMORY).allocFixed(phdr.vaddr, phdr.memsz);
            memcpy(nucleus.memory.ptr(phdr.vaddr), &m_elf[phdr.offset], phdr.filesz);
            // TODO: Static function analysis?
            break;

        case PT_TLS:
            //Emu.SetTLSData(offset + phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);

        case PT_PROC_PARAM:
            if (!phdr.filesz) {
                break;
            }
            // TODO: sys_process stuff
            break;

        case PT_PROC_PRX_PARAM:
            if (!phdr.filesz) {
                break;
            }
            // TODO: sys_prx stuff
            break;
        }
    }
    return true;
}

bool SELFLoader::load_prx(sys_prx_t& prx)
{
    if (!m_elf) {
        return false;
    }

    const auto& ehdr = (Elf64_Ehdr&)m_elf[0];
    u32 base_addr;

    // Loading program header table
    for (u64 i = 0; i < ehdr.phnum; i++) {
        const auto& phdr = (Elf64_Phdr&)m_elf[ehdr.phoff + i*sizeof(Elf64_Phdr)];

        if (phdr.type == PT_LOAD) {
            if (!phdr.memsz) {
                break;
            }

            const u32 addr = nucleus.memory(SEG_MAIN_MEMORY).alloc(phdr.memsz, 0x10000);
            memcpy(nucleus.memory.ptr(addr), &m_elf[phdr.offset], phdr.filesz);

            // Add information for PRX Object
            sys_prx_segment_t segment;
            segment.align = (u32)phdr.align;
            segment.size_file = (u32)phdr.filesz;
            segment.size_memory = (u32)phdr.memsz;
            segment.initial_addr = (u32)phdr.vaddr;
            segment.addr = addr;
            prx.segments.push_back(segment);

            // Get FNID / addr pairs
            if (phdr.paddr != 0) {
                const auto& module = (sys_prx_module_info_t&)m_elf[phdr.paddr];
                prx.name = module.name;
                prx.version = module.version;

                u32 offset = module.exports_start;
                while (offset < module.exports_end) {
                    const auto& library = (sys_prx_library_info_t&)m_elf[phdr.offset + offset];
                    offset += library.size;

                    sys_prx_library_t lib;
                    if (library.name_addr) {
                        lib.name = &m_elf[phdr.offset + library.name_addr];
                    }
                    for (u32 i = 0; i < library.num_func; i++) {
                        const u32 fnid = nucleus.memory.read32(addr + library.fnid_addr + 4*i);
                        const u32 stub = nucleus.memory.read32(addr + library.fstub_addr + 4*i);
                        lib.exports[fnid] = stub;
                    }
                    prx.libraries.push_back(lib);
                }
            }

            // Update offsets according to the base address where the PRX was allocated
            // TODO: Probably hardcoding i==1 isn't a good idea.
            if (i == 1) {
                // Update FNID / addr pairs
                for (auto& lib : prx.libraries) {
                    for (auto& stub : lib.exports) {
                        u32 value = stub.second;
                        for (const auto& segment : prx.segments) {
                            if (segment.initial_addr <= value && value < segment.initial_addr + segment.size_memory) {
                                value = value + (segment.addr - segment.initial_addr);
                            }
                        }
                        stub.second = value;
                    }
                }
            }
        }

        if (phdr.type == PT_SCE_PPURELA) {
            for (u32 i = 0; i < phdr.filesz; i += sizeof(sys_prx_relocation_info_t)) {
                const auto& rel = (sys_prx_relocation_info_t&)m_elf[phdr.offset + i];
                if (rel.type != 1) {
                    continue;
                }
                const u32 addr = prx.segments[1].addr + rel.offset;
                const u32 value = prx.segments[rel.index].addr + rel.ptr;
                nucleus.memory.write32(addr, value);
            }
        }
    }
    return true;
}

void SELFLoader::close()
{
    delete[] m_elf;
    delete[] m_self;
    m_elf = nullptr;
    m_self = nullptr;
    m_elf_size = 0;
    m_self_size = 0;
}

bool SELFLoader::decryptMetadata()
{
    aes_context aes;
    const auto& sce_header = (SceHeader&)m_self[0x0];
    const auto& self_header = (SelfHeader&)m_self[0x20];
    const auto& app_info = (AppInfo&)m_self[0x70];
    const auto* key = getSelfKey(app_info.self_type, app_info.version, sce_header.flags);

    if (!key) {
        return false;
    }

    auto& meta_info = (MetadataInfo&)m_self[sizeof(SceHeader) + sce_header.meta];
    
    // Decrypt NPDRM Layer if necessary
    u8 npdrm_key[0x10];
    u8 npdrm_iv[0x10] = {};
    u32 offset = self_header.controloff;
    while (offset < self_header.controloff + self_header.controlsize) {
        const auto& ctrl = (ControlInfo&)m_self[offset];
        offset += ctrl.size;
        if (ctrl.type != 3) {
            continue;
        }
 
        switch (ctrl.npdrm.license.ToLE()) {
        case 1: // Network license
        case 2: // Local license
            nucleus.log.error(LOG_LOADER, "NPDRM Network / Local licenses not yet supported");
            break;
        case 3: // Free license
            memcpy(npdrm_key, NP_KLIC_FREE, 0x10);
            break;
        }

        // Decrypt our key with NP_KLIC_KEY and use it to decrypt the NPDRM Layer of the Metadata Info
        aes_setkey_dec(&aes, NP_KLIC_KEY, 128);
        aes_crypt_ecb(&aes, AES_DECRYPT, npdrm_key, npdrm_key);
        aes_setkey_dec(&aes, npdrm_key, 128);
        aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(MetadataInfo), npdrm_iv, (u8*)&meta_info, (u8*)&meta_info);
        break;
    }

    // Decrypt Metadata Info
    u8 metadata_iv[0x10];
    memcpy(metadata_iv, key->riv, 0x10);
    aes_setkey_dec(&aes, key->erk, 256);
    aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(MetadataInfo), metadata_iv, (u8*)&meta_info, (u8*)&meta_info);

    // Decrypt Metadata Headers (Metadata Header + Metadata Section Headers)
    u8 ctr_stream_block[0x10];
    u8* meta_headers = (u8*)&m_self[sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo)];
    u32 meta_headers_size = sce_header.hsize - (sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo));
    size_t ctr_nc_off = 0;
    aes_setkey_enc(&aes, meta_info.key, 128);
    aes_crypt_ctr(&aes, meta_headers_size, &ctr_nc_off, meta_info.iv, ctr_stream_block, meta_headers, meta_headers);

    return true;
}

u32 SELFLoader::getDecryptedElfSize()
{
    u32 size = 0;
    const auto& sce_header = (SceHeader&)m_self[0x0];
    const auto& self_header = (SelfHeader&)m_self[0x20];
    const auto& ehdr = (Elf64_Ehdr&)m_self[self_header.elfoff];

    // Check the maximum offset referenced by the PHDRs in the Metadata Headers
    const u32 meta_header_off = sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo);
    const auto& meta_header = (MetadataHeader&)m_self[meta_header_off];
    for (u32 i = 0; i < meta_header.section_count; i++) {
        const auto& meta_shdr = (MetadataSectionHeader&)m_self[meta_header_off + sizeof(MetadataHeader) + i*sizeof(MetadataSectionHeader)];
        const auto& meta_phdr = (Elf64_Phdr&)m_self[self_header.phdroff + meta_shdr.program_idx * sizeof(Elf64_Phdr)];
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
    if (size < ehdr.shoff + ehdr.shnum * sizeof(Elf64_Shdr)) {
        size = ehdr.shoff + ehdr.shnum * sizeof(Elf64_Shdr);
    }
    return size;
}

bool SELFLoader::decrypt()
{
    if (!m_self) {
        return false;
    }

    const auto& sce_header = (SceHeader&)m_self[0x0];
    const auto& self_header = (SelfHeader&)m_self[0x20];

    /**
     * Debug SELF
     */
    if (sce_header.flags == 0x8000) {
        m_elf_size = m_self_size - sce_header.hsize;
        m_elf = new char[m_elf_size];
        memcpy(m_elf, m_self + sce_header.hsize, m_elf_size);
    }

    /**
     * Retail SELF
     */
    else {
        // Get Metadata Information
        decryptMetadata();
        const u32 meta_header_off = sizeof(SceHeader) + sce_header.meta + sizeof(MetadataInfo);
        const auto& meta_header = (MetadataHeader&)m_self[meta_header_off];
        const u8* data_keys = (u8*)&m_self[meta_header_off + sizeof(MetadataHeader) + meta_header.section_count * sizeof(MetadataSectionHeader)];

        // Get ELF size and allocate/initialize it
        m_elf_size = getDecryptedElfSize();
        m_elf = new char[m_elf_size]();

        // Copy the EHDR, PHDRs and SHDRs of the ELF
        const auto& ehdr = (Elf64_Ehdr&)m_self[self_header.elfoff];
        char* self_phdrs = &m_self[self_header.phdroff];
        char* self_shdrs = &m_self[self_header.shdroff];
        char* elf_phdrs = &m_elf[ehdr.phoff];
        char* elf_shdrs = &m_elf[ehdr.shoff];
        memcpy(&m_elf[0], &ehdr, sizeof(Elf64_Ehdr));
        memcpy(elf_phdrs, self_phdrs, ehdr.phnum * sizeof(Elf64_Phdr));
        memcpy(elf_shdrs, self_shdrs, ehdr.shnum * sizeof(Elf64_Shdr));

        // Write Data
        aes_context aes;
        for (u32 i = 0; i < meta_header.section_count; i++) {
            const auto& meta_shdr = (MetadataSectionHeader&)m_self[meta_header_off + sizeof(MetadataHeader) + i*sizeof(MetadataSectionHeader)];
            const auto& meta_phdr = (Elf64_Phdr&)m_self[self_header.phdroff + meta_shdr.program_idx * sizeof(Elf64_Phdr)];
            // Check if PHDR type
            if (meta_shdr.type != 2) {
                continue;
            }

            u8* data_decrypted = new u8[meta_shdr.data_size.ToLE()]();
            u8* data_decompressed = new u8[meta_phdr.filesz.ToLE()]();

            // Decrypt if necessary
            if (meta_shdr.encrypted == 3) {
                u8 data_key[0x10];
                u8 data_iv[0x10];
                memcpy(data_key, data_keys + meta_shdr.key_idx * 0x10, 0x10);
                memcpy(data_iv, data_keys + meta_shdr.iv_idx * 0x10, 0x10);
                memcpy(data_decrypted, &m_self[meta_shdr.data_offset], meta_shdr.data_size);

                // Perform AES-CTR encryption on the data
                u8 ctr_stream_block[0x10] = {};
                size_t ctr_nc_off = 0;
                aes_setkey_enc(&aes, data_key, 128);
                aes_crypt_ctr(&aes, meta_shdr.data_size, &ctr_nc_off, data_iv, ctr_stream_block, data_decrypted, data_decrypted);
            }

            // Decompress if necessary
            if (meta_shdr.compressed == 2) {
                unsigned long length = meta_phdr.filesz;
                uncompress(data_decompressed, &length, data_decrypted, (u32)meta_shdr.data_size);
                memcpy(&m_elf[meta_phdr.offset], data_decompressed, meta_phdr.filesz);
            }
            else {
                memcpy(&m_elf[meta_phdr.offset], data_decrypted, meta_shdr.data_size);
            }

            delete data_decrypted;
            delete data_decompressed;
        }
    }
    return true;
}

u16 SELFLoader::getMachine()
{
    if (!m_elf) {
        return MACHINE_UNKNOWN;
    }
    const auto& ehdr = (Elf64_Ehdr&)m_elf[0];
    return ehdr.machine;
}

u64 SELFLoader::getEntry()
{
    if (!m_elf) {
        return 0;
    }
    const auto& ehdr = (Elf64_Ehdr&)m_elf[0];
    return ehdr.entry;
}
