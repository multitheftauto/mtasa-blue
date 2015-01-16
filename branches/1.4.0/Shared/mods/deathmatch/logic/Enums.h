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


namespace EPlayerScreenShotResult
{
    enum EPlayerScreenShotResultType
    {
        NONE,
        SUCCESS,
        MINIMIZED,
        DISABLED,
        ERROR_,
    };
}
using EPlayerScreenShotResult::EPlayerScreenShotResultType;

DECLARE_ENUM( EPlayerScreenShotResult::EPlayerScreenShotResultType )


namespace EDebugHook
{
    enum EDebugHookType
    {
        PRE_EVENT,
        POST_EVENT,
        PRE_FUNCTION,
        POST_FUNCTION,
    };
}
using EDebugHook::EDebugHookType;

DECLARE_ENUM( EDebugHook::EDebugHookType );


enum eEulerRotationOrder
{
    EULER_DEFAULT,
    EULER_ZXY,
    EULER_ZYX,
    EULER_MINUS_ZYX,
};

DECLARE_ENUM( eEulerRotationOrder );

DECLARE_ENUM( EHashFunction::EHashFunctionType );
DECLARE_ENUM( ePacketID );
