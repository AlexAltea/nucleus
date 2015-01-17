/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Return codes
enum {
    CELL_VIDEO_OUT_SUCCEEDED                      = 0,
    CELL_VIDEO_OUT_ERROR_NOT_IMPLEMENTED          = 0x8002b220,
    CELL_VIDEO_OUT_ERROR_ILLEGAL_CONFIGURATION    = 0x8002b221,
    CELL_VIDEO_OUT_ERROR_ILLEGAL_PARAMETER        = 0x8002b222,
    CELL_VIDEO_OUT_ERROR_PARAMETER_OUT_OF_RANGE   = 0x8002b223,
    CELL_VIDEO_OUT_ERROR_DEVICE_NOT_FOUND         = 0x8002b224,
    CELL_VIDEO_OUT_ERROR_UNSUPPORTED_VIDEO_OUT    = 0x8002b225,
    CELL_VIDEO_OUT_ERROR_UNSUPPORTED_DISPLAY_MODE = 0x8002b226,
    CELL_VIDEO_OUT_ERROR_CONDITION_BUSY           = 0x8002b227,
    CELL_VIDEO_OUT_ERROR_VALUE_IS_NOT_SET         = 0x8002b228,
};

// Constants
enum CellVideoOut {
    CELL_VIDEO_OUT_PRIMARY   = 0,
    CELL_VIDEO_OUT_SECONDARY = 1,
};

enum CellVideoOutOutputState {
    CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED   = 0,
    CELL_VIDEO_OUT_OUTPUT_STATE_DISABLED  = 1,
    CELL_VIDEO_OUT_OUTPUT_STATE_PREPARING = 2,
};

enum CellVideoOutColorSpace {
    CELL_VIDEO_OUT_COLOR_SPACE_RGB   = 0x01,
    CELL_VIDEO_OUT_COLOR_SPACE_YUV   = 0x02,
    CELL_VIDEO_OUT_COLOR_SPACE_XVYCC = 0x04,
};

enum CellVideoOutResolutionId {
    CELL_VIDEO_OUT_RESOLUTION_UNDEFINED  = 0,
    CELL_VIDEO_OUT_RESOLUTION_1080       = 1,
    CELL_VIDEO_OUT_RESOLUTION_720        = 2,
    CELL_VIDEO_OUT_RESOLUTION_480        = 4,
    CELL_VIDEO_OUT_RESOLUTION_576        = 5,
    CELL_VIDEO_OUT_RESOLUTION_1600x1080  = 10,
    CELL_VIDEO_OUT_RESOLUTION_1440x1080  = 11,
    CELL_VIDEO_OUT_RESOLUTION_1280x1080  = 12,
    CELL_VIDEO_OUT_RESOLUTION_960x1080   = 13,
    CELL_VIDEO_OUT_RESOLUTION_720_3D_FRAME_PACKING        = 0x81,
    CELL_VIDEO_OUT_RESOLUTION_1024x720_3D_FRAME_PACKING   = 0x88,
    CELL_VIDEO_OUT_RESOLUTION_960x720_3D_FRAME_PACKING    = 0x89,
    CELL_VIDEO_OUT_RESOLUTION_800x720_3D_FRAME_PACKING    = 0x8a,
    CELL_VIDEO_OUT_RESOLUTION_640x720_3D_FRAME_PACKING    = 0x8b,
    CELL_VIDEO_OUT_RESOLUTION_720_DUALVIEW_FRAME_PACKING  = 0x91,
    CELL_VIDEO_OUT_RESOLUTION_720_SIMULVIEW_FRAME_PACKING = 0x91,
    CELL_VIDEO_OUT_RESOLUTION_1024x720_DUALVIEW_FRAME_PACKING   = 0x98,
    CELL_VIDEO_OUT_RESOLUTION_1024x720_SIMULVIEW_FRAME_PACKING  = 0x98,
    CELL_VIDEO_OUT_RESOLUTION_960x720_DUALVIEW_FRAME_PACKING    = 0x99,
    CELL_VIDEO_OUT_RESOLUTION_960x720_SIMULVIEW_FRAME_PACKING   = 0x99,
    CELL_VIDEO_OUT_RESOLUTION_800x720_DUALVIEW_FRAME_PACKING    = 0x9a,
    CELL_VIDEO_OUT_RESOLUTION_800x720_SIMULVIEW_FRAME_PACKING   = 0x9a,
    CELL_VIDEO_OUT_RESOLUTION_640x720_DUALVIEW_FRAME_PACKING    = 0x9b,
    CELL_VIDEO_OUT_RESOLUTION_640x720_SIMULVIEW_FRAME_PACKING   = 0x9b,
};

enum CellVideoOutScanMode {
    CELL_VIDEO_OUT_SCAN_MODE_INTERLACE   = 0,
    CELL_VIDEO_OUT_SCAN_MODE_PROGRESSIVE = 1,
};

enum CellVideoOutDisplayConversion {
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_NONE                           = 0x00,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_WXGA                        = 0x01,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_SXGA                        = 0x02,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_WUXGA                       = 0x03,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_1080                        = 0x05,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_REMOTEPLAY                  = 0x10,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_720_3D_FRAME_PACKING        = 0x80,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_720_DUALVIEW_FRAME_PACKING  = 0x81,
    CELL_VIDEO_OUT_DISPLAY_CONVERSION_TO_720_SIMULVIEW_FRAME_PACKING = 0x81,
};

enum CellVideoOutDisplayAspect {
    CELL_VIDEO_OUT_ASPECT_AUTO = 0,
    CELL_VIDEO_OUT_ASPECT_4_3  = 1,
    CELL_VIDEO_OUT_ASPECT_16_9 = 2,
};

enum CellVideoOutRefreshRate {
    CELL_VIDEO_OUT_REFRESH_RATE_AUTO    = 0x0000,
    CELL_VIDEO_OUT_REFRESH_RATE_59_94HZ = 0x0001,
    CELL_VIDEO_OUT_REFRESH_RATE_50HZ    = 0x0002,
    CELL_VIDEO_OUT_REFRESH_RATE_60HZ    = 0x0004,
    CELL_VIDEO_OUT_REFRESH_RATE_30HZ    = 0x0008,
};

// Data types
struct CellVideoOutDisplayMode {
    u8 resolutionId;
    u8 scanMode;
    u8 conversion;
    u8 aspect;
    u8 reserved[2];
    be_t<u16> refreshRates;
};

struct CellVideoOutState {
    u8 state;
    u8 colorSpace;
    u8 reserved[6];
    CellVideoOutDisplayMode displayMode;
};

struct CellVideoOutConfiguration {
    u8 resolutionId;
    u8 format;
    u8 aspect;
    u8 reserved[9];
    be_t<u32> pitch;
};

struct CellVideoOutOption {
    be_t<u32> reserved;
};

struct CellVideoOutResolution {
    be_t<u16> width;
    be_t<u16> height;
};

// Functions
s32 cellVideoOutConfigure(u32 videoOut, CellVideoOutConfiguration* config, CellVideoOutOption* option, u32 waitForEvent);
s32 cellVideoOutGetDeviceInfo();
s32 cellVideoOutGetResolution(u32 resolutionId, CellVideoOutResolution* resolution);
s32 cellVideoOutGetState(u32 videoOut, u32 deviceIndex, CellVideoOutState* state);
