/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/MarkerDisplay.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>

enum class MarkerDisplay : std::uint8_t
{
    MARKER_DISPLAY_NEITHER = 0,            //  BLIPDISPLAY_NEITHER
    MARKER_DISPLAY_MARKERONLY,             //  BLIPDISPLAY_MARKERONLY
    MARKER_DISPLAY_BLIPONLY,               //  MARKER_DISPLAY_BLIPONLY
    MARKER_DISPLAY_BOTH                    //  BLIPDISPLAY_BOTH
};
