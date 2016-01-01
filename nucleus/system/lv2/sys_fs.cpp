/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_fs.h"
#include "nucleus/emulator.h"
#include "nucleus/filesystem/filesystem_virtual.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/lv2.h"

#include <cstring>

namespace sys {

// SysCalls
S32 sys_fs_open(const S8* path, S32 flags, BE<S32>* fd, U64 mode, const void* arg, U64 size) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Create file
    if (flags & CELL_FS_O_CREAT) {
        lv2.vfs.createFile(path);
    }

    // Access mode
    fs::OpenMode openMode;
    switch (flags & CELL_FS_O_ACCMODE) {
    case CELL_FS_O_RDONLY:
        openMode = fs::Read;
        break;

    case CELL_FS_O_WRONLY:
        if (flags & CELL_FS_O_APPEND) {
            openMode = fs::WriteAppend;
        }
        else if (flags & CELL_FS_O_EXCL) {
            openMode = fs::Write;
        }
        else if (flags & CELL_FS_O_TRUNC) {
            openMode = fs::Write;
        }
        else {
            openMode = fs::Write;
        }
        break;

    case CELL_FS_O_RDWR:
        if (flags & CELL_FS_O_TRUNC) {
            lv2.vfs.createFile(path);
        }
        openMode = fs::ReadWrite;
    }

    if (!lv2.vfs.existsFile(path)) {
        return CELL_ENOENT;
    }

    auto* file = new sys_fs_t();
    file->type = CELL_FS_S_IFREG;
    file->path = path;
    file->file = lv2.vfs.openFile(path, openMode);

    *fd = lv2.objects.add(file, SYS_FS_FD_OBJECT);
    return CELL_OK;
}

S32 sys_fs_read(S32 fd, void* buf, U64 nbytes, BE<U64>* nread) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* descriptor = lv2.objects.get<sys_fs_t>(fd);
    auto* file = descriptor->file;

    *nread = file->read(buf, nbytes);
    return CELL_OK;
}

S32 sys_fs_write(S32 fd, const void* buf, U64 nbytes, BE<U64>* nwrite) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* descriptor = lv2.objects.get<sys_fs_t>(fd);
    auto* file = descriptor->file;

    *nwrite = file->write(buf, nbytes);
    return CELL_OK;
}

S32 sys_fs_close(S32 fd) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* descriptor = lv2.objects.get<sys_fs_t>(fd);
    delete descriptor->file;

    return CELL_OK;
}

S32 sys_fs_fstat(S32 fd, sys_fs_stat_t* sb) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (sb == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    auto* descriptor = lv2.objects.get<sys_fs_t>(fd);
    auto* file = descriptor->file;

    auto attributes = file->attributes();
    sb->st_atime = attributes.timestamp_access;
    sb->st_ctime = attributes.timestamp_create;
    sb->st_mtime = attributes.timestamp_write;
    sb->st_size = attributes.size;
    sb->st_blksize = attributes.blocksize;
    sb->st_gid = 0; // TODO
    sb->st_uid = 0; // TODO
    sb->st_mode = 0; // TODO

    return CELL_OK;
}

S32 sys_fs_stat(const S8* path, sys_fs_stat_t* sb) {
    // Check requisites
    if (path == nucleus.memory->ptr(0) || sb == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    auto attributes = nucleus.sys->vfs.getFileAttributes(path);
    sb->st_atime = attributes.timestamp_access;
    sb->st_ctime = attributes.timestamp_create;
    sb->st_mtime = attributes.timestamp_write;
    sb->st_size = attributes.size;
    sb->st_blksize = attributes.blocksize;
    sb->st_gid = 0; // TODO
    sb->st_uid = 0; // TODO
    sb->st_mode = 0; // TODO

    return CELL_OK;
}

S32 sys_fs_fcntl() {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    logger.warning(LOG_HLE, "LV2 Syscall (0x331) called: sys_fs_fcntl");
    return CELL_OK;
}

S32 sys_fs_lseek(S32 fd, S64 offset, S32 whence, U64 *pos) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* descriptor = lv2.objects.get<sys_fs_t>(fd);
    auto* file = descriptor->file;

    *pos = file->seek(offset, fs::SeekSet);
    return CELL_OK;
}

}  // namespace sys
