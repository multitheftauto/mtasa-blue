/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/MarkerType.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>

enum class MarkerType : std::uint8_t
{
    MARKER_TYPE_UNUSED,
    MARKER_TYPE_CAR,
    MARKER_TYPE_CHAR,
    MARKER_TYPE_OBJECT,
    MARKER_TYPE_COORDS,
    MARKER_TYPE_CONTACT,
    MARKER_TYPE_SEARCHLIGHT,
    MARKER_TYPE_PICKUP,
    MARKER_TYPE_AIRSTRIP
};
