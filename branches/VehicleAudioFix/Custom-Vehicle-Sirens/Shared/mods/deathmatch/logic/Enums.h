/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Enums.h
*  PURPOSE:     Client/server shared enum definitions
*
*****************************************************************************/

namespace EEventPriority
{
    enum EEventPriorityType
    {
        LOW,
        NORMAL,
        HIGH,
    };
}
using EEventPriority::EEventPriorityType;

DECLARE_ENUM( EEventPriority::EEventPriorityType )
