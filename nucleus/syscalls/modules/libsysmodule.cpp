/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "libsysmodule.h"
#include "nucleus/emulator.h"

s32 cellVideoOutConfigure(u32 videoOut, CellVideoOutConfiguration* config, CellVideoOutOption* option, u32 waitForEvent)
{
    if (videoOut != CELL_VIDEO_OUT_PRIMARY) {
        return CELL_VIDEO_OUT_ERROR_UNSUPPORTED_VIDEO_OUT;
    }

    return CELL_VIDEO_OUT_SUCCEEDED;
}

s32 cellVideoOutGetState(u32 videoOut, u32 deviceIndex, CellVideoOutState* state)
{
    if (deviceIndex > 0) {
        return CELL_VIDEO_OUT_ERROR_DEVICE_NOT_FOUND;
    }
    if (videoOut != CELL_VIDEO_OUT_PRIMARY) {
        return CELL_VIDEO_OUT_ERROR_UNSUPPORTED_VIDEO_OUT;
    }

    state->state = CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED;
    state->colorSpace = CELL_VIDEO_OUT_COLOR_SPACE_RGB;
    state->displayMode.resolutionId = CELL_VIDEO_OUT_RESOLUTION_480;
    state->displayMode.scanMode = CELL_VIDEO_OUT_SCAN_MODE_INTERLACE;
    state->displayMode.conversion = CELL_VIDEO_OUT_DISPLAY_CONVERSION_NONE;
    state->displayMode.aspect = CELL_VIDEO_OUT_ASPECT_4_3;
    state->displayMode.refreshRates = CELL_VIDEO_OUT_REFRESH_RATE_50HZ;

    return CELL_VIDEO_OUT_SUCCEEDED;
}

s32 cellVideoOutGetResolution(u32 resolutionId, CellVideoOutResolution* resolution)
{
    switch (resolutionId) {
    case CELL_VIDEO_OUT_RESOLUTION_1080:
        resolution->width = 1920;
        resolution->height = 1080;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_720:
        resolution->width = 1280;
        resolution->height = 720;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_480:
        resolution->width = 720;
        resolution->height = 480;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_576:
        resolution->width = 720;
        resolution->height = 576;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_1600x1080:
        resolution->width = 1600;
        resolution->height = 1080;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_1440x1080:
        resolution->width = 1440;
        resolution->height = 1080;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_1280x1080:
        resolution->width = 1280;
        resolution->height = 1080;
        break;

    case CELL_VIDEO_OUT_RESOLUTION_960x1080:
        resolution->width = 960;
        resolution->height = 1080;
        break;

    default:
        nucleus.log.warning(LOG_HLE, "cellVideoOutGetResolution: Wrong resolutionId (%d), probably unimplemented.", resolutionId);
    }

    return CELL_VIDEO_OUT_SUCCEEDED;
}
