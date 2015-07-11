/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/filesystem/filesystem.h"

#include <string>

namespace sys {

// Constants
enum {
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
    U8 d_type;
    U8 d_namlen;
    S8 d_name[CELL_MAX_FS_FILE_NAME_LENGTH + 1];
};

struct sys_fs_stat_t
{
    BE<U32> st_mode;
    BE<S32> st_uid;
    BE<S32> st_gid;
    BE<U64> st_atime;
    BE<U64> st_mtime;
    BE<U64> st_ctime;
    BE<U64> st_size;
    BE<U64> st_blksize;
};

// Auxiliary classes
struct sys_fs_t
{
    S32 type;
    std::string path;

    // Pointer to the corresponding type: File or Directory
    union {
        fs::File* file;
    };
};

// SysCalls
S32 sys_fs_open(const S8* path, S32 flags, BE<S32>* fd, U64 mode, const void* arg, U64 size);
S32 sys_fs_read(S32 fd, void* buf, U64 nbytes, BE<U64>* nread);
S32 sys_fs_write(S32 fd, const void* buf, U64 nbytes, BE<U64>* nwrite);
S32 sys_fs_close(S32 fd);
S32 sys_fs_opendir(const S8* path, S32* fd);
S32 sys_fs_readdir(S32 fd, sys_fs_dirent_t* dir, BE<U64>* nread);
S32 sys_fs_closedir(S32 fd);
S32 sys_fs_stat(const S8* path, sys_fs_stat_t* sb);
S32 sys_fs_fstat(S32 fd, sys_fs_stat_t* sb);
S32 sys_fs_link(const S8* path, const S8* arg0);
S32 sys_fs_mkdir(const S8* path, U32 mode);
S32 sys_fs_rename(const S8* from, const S8* to);
S32 sys_fs_rmdir(const S8* path);
S32 sys_fs_unlink(const S8* path);
S32 sys_fs_fcntl();
S32 sys_fs_lseek(S32 fd, S64 offset, S32 whence, U64 *pos);

}  // namespace sys
