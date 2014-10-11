/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "filesystem.h"

const char* gtOpenMode(OpenMode mode)
{
    switch (mode) {
    case Read:        return "rb";
    case Write:       return "wb";
    case ReadWrite:   return "r+b";
    }

    return "r";
}

const int gtSeekMode(SeekMode mode)
{
    switch (mode) {
    case SeekSet:  return SEEK_SET;
    case SeekCur:  return SEEK_CUR;
    case SeekEnd:  return SEEK_END;
    }

    return SEEK_SET;
}