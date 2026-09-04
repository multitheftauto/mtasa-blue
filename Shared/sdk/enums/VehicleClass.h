/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/enums/VehicleClass.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>

namespace VehicleClass
{
    enum Enum : std::uint8_t
    {
        AUTOMOBILE,
        MONSTER_TRUCK,
        QUAD,
        HELI,
        PLANE,
        BOAT,
        TRAIN,
        FAKE_HELI,
        FAKE_PLANE,
        BIKE,
        BMX,
        TRAILER,
    };
}
