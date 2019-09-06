/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Enums.h
 *  PURPOSE:     Client/server shared enum definitions
 *
 *****************************************************************************/
#pragma once

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
DECLARE_ENUM_CLASS(PasswordHashFunction);
DECLARE_ENUM_CLASS(StringEncryptFunction);

DECLARE_ENUM(ePacketID);

enum eCullMode
{
    CULL_INSIDE,
    CULL_OUTSIDE,
    CULL_BOTH,
};
DECLARE_ENUM(eCullMode);

enum eFBXObjectType
{
    FBX_OBJECT_ALL = -1,
    FBX_OBJECT_ROOT,
    FBX_OBJECT_GEOMETRY,
    FBX_OBJECT_MATERIAL,
    FBX_OBJECT_MESH,
    FBX_OBJECT_TEXTURE,
    FBX_OBJECT_LIMB_NODE,
    FBX_OBJECT_NULL_NODE,
    FBX_OBJECT_NODE_ATTRIBUTE,
    FBX_OBJECT_CLUSTER,
    FBX_OBJECT_SKIN,
    FBX_OBJECT_ANIMATION_STACK,
    FBX_OBJECT_ANIMATION_LAYER,
    FBX_OBJECT_ANIMATION_CURVE,
    FBX_OBJECT_ANIMATION_CURVE_NODE,
};
DECLARE_ENUM(eFBXObjectType);
