/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/DoorState.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>

enum class DoorState : std::uint8_t
{
    DOOR_NOTHING,
    DOOR_HIT_MAX_END,
    DOOR_HIT_MIN_END,
    DOOR_POP_OPEN,
    DOOR_SLAM_SHUT
};
