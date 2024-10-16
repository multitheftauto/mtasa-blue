/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/C2DEffect.h
 *  PURPOSE:     Game 2dfx interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *
 *****************************************************************************/

#pragma once

class C2DEffectSA;
class C2DEffect;
class CVector;
class C2DEffectSAInterface;

enum class e2dEffectType : std::uint8_t
{
    LIGHT = 0,
    PARTICLE,
    UNKNOWN,
    ATTRACTOR,
    SUN_GLARE,
    FURNITURE,
    ENEX,
    ROADSIGN,
    TRIGGER_POINT,
    COVER_POINT,
    ESCALATOR,

    NONE,
};

enum class e2dCoronaFlashType : std::uint8_t
{
    DEFAULT = 0,
    RANDOM,
    RANDOM_WHEN_WET,
    ANIM_SPEED_4X,
    ANIM_SPEED_2X,
    ANIM_SPEED_1X,
    WARNLIGHT, // Used on model nt_roadblockci
    TRAFFICLIGHT,
    TRAINCROSSING,
    UNUSED,
    ONLY_RAIN,
    ON5_OFF5,
    ON6_OFF4,
    ON4_OFF6,
};

enum class e2dEffectProperty
{
    // light properties
    FAR_CLIP_DISTANCE = 0,
    LIGHT_RANGE,
    CORONA_SIZE,
    SHADOW_SIZE,
    SHADOW_MULT,
    FLASH_TYPE,
    CORONA_REFLECTION,
    FLARE_TYPE,
    SHADOW_DISTANCE,
    OFFSET_X,
    OFFSET_Y,
    OFFSET_Z,
    COLOR,
    CORONA_NAME,
    SHADOW_NAME,
    FLAGS,            // for light & roadsign

    // particle properties
    PRT_NAME,

    // roadsign properties
    SIZE_X,
    SIZE_Y,
    ROT_X,
    ROT_Y,
    ROT_Z,
    TEXT,
    TEXT_2,
    TEXT_3,
    TEXT_4,

    // escalator properties
    BOTTOM_X,
    BOTTOM_Y,
    BOTTOM_Z,
    TOP_X,
    TOP_Y,
    TOP_Z,
    END_X,
    END_Y,
    END_Z,
    DIRECTION,
};

enum class e2dAttractorType : std::int8_t
{
    UNDEFINED = -1,
    ATM = 0,
    SEAT,
    STOP,
    PIZZA,
    SHELTER,
    TRIGGER_SCRIPT,
    LOOK_AT,
    SCRIPTED,
    PARK,
    STEP,
};

class C2DEffects
{
public:
    virtual C2DEffect* Create(std::uint32_t model, const CVector& position, const e2dEffectType& type) = 0;
    virtual void         Destroy(C2DEffect* effect) = 0;
    virtual C2DEffectSA* Get(C2DEffectSAInterface* effectInterface) const = 0;
    virtual void         AddToList(C2DEffectSA* effect) = 0;
    virtual void        RemoveFromList(C2DEffectSA* effect) = 0;
};
