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
