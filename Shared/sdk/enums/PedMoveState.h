/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/PedMoveState.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace PedMoveState
{
    enum Enum
    {
        PEDMOVE_NONE = 0,
        PEDMOVE_STILL,
        PEDMOVE_TURN_L,
        PEDMOVE_TURN_R,
        PEDMOVE_WALK,
        PEDMOVE_JOG,
        PEDMOVE_RUN,
        PEDMOVE_SPRINT,
    };
}
