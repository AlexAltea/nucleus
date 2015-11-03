/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace fs {

enum OpenMode {
    Read     = (1 << 0),
    Write    = (1 << 1),
    Append   = (1 << 2),
    ReadWrite = Read | Write,
    WriteAppend = Write | Append,
};

enum SeekMode {
    SeekSet,  // Beginning of the file
    SeekCur,  // Current position of the file pointer
    SeekEnd,  // End of file
};

using Position = S64;
using Size = U64;

class File {
public:
    struct Attributes {
        U64 timestamp_access;
        U64 timestamp_create;
        U64 timestamp_write;
        U64 size;
        U32 blocksize;
    };

    virtual ~File() = default;

    /**
     * Read from the file
     * @param[out]  dst   Buffer where the read contents will be stored
     * @param[in]   size  Number of bytes to be read
     * @return            Number of read bytes
     */
    virtual Size read(void* dst, Size size) = 0;

    /**
     * Write to the file
     * @param[in]   src   Buffer where the written contents come from
     * @param[in]   size  Number of bytes to write
     * @return            Number of written bytes
     */
    virtual Size write(const void* src, Size size) = 0;

    /**
     * Seek a certain position in the file
     * @param[in]   pos   Position offset
     * @param[in]   mode  Seek mode
     * @return            New position
     */
    virtual Position seek(Position pos, SeekMode mode) = 0;

    /**
     * Tell the current offset in the file
     * @return            Current position
     */
    virtual Position tell() = 0;

    /**
     * Obtain the current attributes of the file
     * @return            Current attributes
     */
    virtual Attributes attributes() = 0;
};

}  // namespace fs
