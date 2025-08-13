/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/VehicleSoundType.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>

enum class VehicleSoundType : std::uint8_t
{
    CAR = 0,
    MOTORCYCLE,
    BICYCLE,
    BOAT,
    HELI,
    PLANE,
    TRAIN = 8,
    TRAILLER,
    SPECIAL,
};
