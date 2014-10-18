/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/filesystem.h"

#include <string>

// Constants
enum
{
    // File mode
    CELL_FS_S_IFDIR   = 0040000,  // Directory
    CELL_FS_S_IFREG   = 0100000,  // Regular
    CELL_FS_S_IFLNK   = 0120000,  // Symbolic link
    CELL_FS_S_IFWHT   = 0160000,  // Unknown
    CELL_FS_S_IRUSR   = 0000400,  // R for owner
    CELL_FS_S_IWUSR   = 0000200,  // W for owner
    CELL_FS_S_IXUSR   = 0000100,  // X for owner
    CELL_FS_S_IRGRP   = 0000040,  // R for group
    CELL_FS_S_IWGRP   = 0000020,  // W for group
    CELL_FS_S_IXGRP   = 0000010,  // X for group
    CELL_FS_S_IROTH   = 0000004,  // R for other
    CELL_FS_S_IWOTH   = 0000002,  // W for other
    CELL_FS_S_IXOTH   = 0000001,  // X for other

    // Open flags
    CELL_FS_O_ACCMODE = 0000003,  // Access mode: Mask
    CELL_FS_O_RDONLY  = 0000000,  // Access mode: Read only
    CELL_FS_O_WRONLY  = 0000001,  // Access mode: Write only
    CELL_FS_O_RDWR    = 0000002,  // Access mode: Read/write
    CELL_FS_O_CREAT   = 0000100,
    CELL_FS_O_EXCL    = 0000200,
    CELL_FS_O_TRUNC   = 0001000,
    CELL_FS_O_APPEND  = 0002000,
    CELL_FS_O_MSELF   = 0010000,

    // Maximum lengths
    CELL_MAX_FS_PATH_LENGTH = 1024,
    CELL_MAX_FS_FILE_NAME_LENGTH = 255,
    CELL_MAX_FS_MP_LENGTH = 31,
};

// Structs
struct sys_fs_dirent_t
{
    u8 d_type;
    u8 d_namlen;
    s8 d_name[CELL_MAX_FS_FILE_NAME_LENGTH + 1];
};

struct sys_fs_stat_t
{
    be_t<u32> st_mode;
    be_t<s32> st_uid;
    be_t<s32> st_gid;
    be_t<u64> st_atime;
    be_t<u64> st_mtime;
    be_t<u64> st_ctime;
    be_t<u64> st_size;
    be_t<u64> st_blksize;
};

// Auxiliary classes
struct sys_fs_t
{
    s32 type;
    std::string path;
    FileSystem* fs;

    // Pointer to the corresponding type: File or Directory
    union {
        File* file;
    };
};

// SysCalls
s32 sys_fs_open(const s8* path, s32 flags, be_t<s32>* fd, u64 mode, const void* arg, u64 size);
s32 sys_fs_read(s32 fd, void* buf, u64 nbytes, be_t<u64>* nread);
s32 sys_fs_write(s32 fd, const void* buf, u64 nbytes, be_t<u64>* nwrite);
s32 sys_fs_close(s32 fd);
s32 sys_fs_opendir(const s8* path, s32* fd);
s32 sys_fs_readdir(s32 fd, sys_fs_dirent_t* dir, be_t<u64>* nread);
s32 sys_fs_closedir(s32 fd);
s32 sys_fs_stat(const s8* path, sys_fs_stat_t* sb);
s32 sys_fs_fstat(s32 fd, sys_fs_stat_t* sb);
s32 sys_fs_link(const s8* path, const s8* arg0);
s32 sys_fs_mkdir(const s8* path, u32 mode);
s32 sys_fs_rename(const s8* from, const s8* to);
s32 sys_fs_rmdir(const s8* path);
s32 sys_fs_unlink(const s8* path);
