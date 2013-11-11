/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Enums.cpp
*  PURPOSE:     Client/server shared enum definitions
*
*****************************************************************************/

#include "StdInc.h"

IMPLEMENT_ENUM_BEGIN( EEventPriority::EEventPriorityType )
    ADD_ENUM ( EEventPriority::LOW,         "low" )
    ADD_ENUM ( EEventPriority::NORMAL,      "normal" )
    ADD_ENUM ( EEventPriority::HIGH,        "high" )
IMPLEMENT_ENUM_END( "event-priority" )

IMPLEMENT_ENUM_BEGIN( EPlayerScreenShotResult::EPlayerScreenShotResultType )
    ADD_ENUM ( EPlayerScreenShotResult::NONE,       "none" )
    ADD_ENUM ( EPlayerScreenShotResult::SUCCESS,    "success" )
    ADD_ENUM ( EPlayerScreenShotResult::MINIMIZED,  "minimized" )
    ADD_ENUM ( EPlayerScreenShotResult::DISABLED,   "disabled" )
    ADD_ENUM ( EPlayerScreenShotResult::ERROR_,     "error" )
IMPLEMENT_ENUM_END( "player-screenshot-result" )
