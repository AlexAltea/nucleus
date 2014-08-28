/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/common.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/vfsLocalFile.h"
#include "nucleus/syscalls/lv2/sys_process.h"
#include "nucleus/syscalls/lv2/sys_prx.h"
#include "loader.h"
#include "self.h"
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

bool SELFLoader::load()
{
    if (!m_elf) {
        return false;
    }

    const Elf64_Ehdr& ehdr = (Elf64_Ehdr&)m_elf[0];

    // Loading program header table
    for (u64 i = 0; i < ehdr.phnum; i++) {
        const Elf64_Phdr& phdr = (Elf64_Phdr&)m_elf[ehdr.phoff + i*sizeof(Elf64_Phdr)];

        switch (phdr.type.ToLE()) {
        case 0x00000001: //LOAD
            if (!phdr.memsz) {
                break;
            }

            nucleus.memory(SEG_MAIN_MEMORY).allocFixed(phdr.vaddr, phdr.memsz);
            memcpy(nucleus.memory[phdr.vaddr], &m_elf[phdr.offset], phdr.filesz);
            // TODO: Static function analysis?
            break;

        case 0x00000007: //TLS
            //Emu.SetTLSData(offset + phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);

        case 0x60000001: //LOOS+1
            if (!phdr.filesz) {
			    break;
            }
            // TODO: sys_process stuff
            break;

        case 0x60000002: //LOOS+2
            if (!phdr.filesz) {
			    break;
            }
            // TODO: sys_prx stuff
            break;
        }
    }

    // Loading section header table
    const Elf64_Shdr& shstr = (Elf64_Shdr&)m_elf[ehdr.shoff + ehdr.shstrndx*sizeof(Elf64_Shdr)];

    for (u64 i = 0; i < ehdr.shnum; i++) {
        const Elf64_Shdr& shdr = (Elf64_Shdr&)m_elf[ehdr.shoff + i*sizeof(Elf64_Shdr)];
        const char* shdr_name = (const char*)m_elf[shstr.offset + shdr.name];
        // TODO ?
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

bool SELFLoader::decrypt() // TODO
{
    if (!m_self) {
        return false;
    }

    const SceHeader& sce_header = (SceHeader&)m_self[0];
    if (isDebug()) {
    }

    return true;
}

bool SELFLoader::isDebug()
{
    if (!m_self) {
        return false;
    }

    const SceHeader& sce_header = (SceHeader&)m_self[0];
    if (sce_header.magic == 'SCE\x00' && sce_header.flags == 0x8000) {
        return true;
    }
    return false;
}

u16 SELFLoader::getMachine()
{
    if (!m_elf) {
        return MACHINE_UNKNOWN;
    }
    const Elf64_Ehdr& ehdr = (Elf64_Ehdr&)m_elf[0];
    return ehdr.machine;
}

u64 SELFLoader::getEntry()
{
    if (!m_elf) {
        return 0;
    }
    const Elf64_Ehdr& ehdr = (Elf64_Ehdr&)m_elf[0];
    return ehdr.entry;
}