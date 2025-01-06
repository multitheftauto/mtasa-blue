/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Enums.h
 *  PURPOSE:     Client/server shared enum definitions
 *
 *****************************************************************************/

#pragma once

#include <net/Packets.h>

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

DECLARE_ENUM(EEventPriority::EEventPriorityType)

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

DECLARE_ENUM(EPlayerScreenShotResult::EPlayerScreenShotResultType)

namespace EDebugHook
{
    enum EDebugHookType
    {
        PRE_EVENT,
        POST_EVENT,
        PRE_FUNCTION,
        POST_FUNCTION,
        PRE_EVENT_FUNCTION,
        POST_EVENT_FUNCTION,
        MAX_DEBUG_HOOK_TYPE
    };
}
using EDebugHook::EDebugHookType;

DECLARE_ENUM(EDebugHook::EDebugHookType);

enum eEulerRotationOrder
{
    EULER_DEFAULT,
    EULER_ZXY,
    EULER_ZYX,
    EULER_MINUS_ZYX,
};

DECLARE_ENUM(eEulerRotationOrder);

DECLARE_ENUM(EHashFunction::EHashFunctionType);
DECLARE_ENUM_CLASS(HashFunctionType);
DECLARE_ENUM_CLASS(PasswordHashFunction);
DECLARE_ENUM_CLASS(StringEncodeFunction);
DECLARE_ENUM_CLASS(KeyPairAlgorithm);
DECLARE_ENUM_CLASS(HmacAlgorithm);

enum class WorldSpecialProperty
{
    HOVERCARS,
    AIRCARS,
    EXTRABUNNY,
    EXTRAJUMP,
    RANDOMFOLIAGE,
    SNIPERMOON,
    EXTRAAIRRESISTANCE,
    UNDERWORLDWARP,
    VEHICLESUNGLARE,
    CORONAZTEST,
    WATERCREATURES,
    BURNFLIPPEDCARS,
    FIREBALLDESTRUCT,
    ROADSIGNSTEXT,
    EXTENDEDWATERCANNONS,
    TUNNELWEATHERBLEND,
    IGNOREFIRESTATE,
    FLYINGCOMPONENTS,
};
DECLARE_ENUM_CLASS(WorldSpecialProperty);

DECLARE_ENUM(ePacketID);
