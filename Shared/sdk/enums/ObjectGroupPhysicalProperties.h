/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/enums/ObjectGroupPhysicalProperties.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace ObjectGroupPhysicalProperties
{
    enum Modifiable
    {
        MASS,
        TURNMASS,
        AIRRESISTANCE,
        ELASTICITY,
        BUOYANCY,
        UPROOTLIMIT,
        COLDAMAGEMULTIPLIER,
        COLDAMAGEEFFECT,
        SPECIALCOLRESPONSE,
        CAMERAAVOID,
        EXPLOSION,
        FXTYPE,
        FXOFFSET,
        FXSYSTEM,
        SMASHMULTIPLIER,
        BREAKVELOCITY,
        BREAKVELOCITYRAND,
        BREAKMODE,
        SPARKSONIMPACT
    };

    enum DamageEffect
    {
        NO_EFFECT = 0,
        CHANGE_MODEL = 1,
        SMASH_COMPLETELY = 20,
        CHANGE_THEN_SMASH = 21,
        BREAKABLE = 200,
        BREAKABLE_REMOVED = 202
    };

    enum CollisionResponse
    {
        NO_RESPONSE,
        LAMPPOST,
        SMALLBOX,
        BIGBOX,
        FENCEPART,
        GRENADE,
        SWINGDOOR,
        LOCKDOOR,
        HANGING,
        POOLBALL
    };

    enum FxType
    {
        NO_FX,
        PLAY_ON_HIT,
        PLAY_ON_DESTROYED,
        PLAY_ON_HIT_DESTROYED
    };

    enum BreakMode
    {
        NOT_BY_GUN,
        BY_GUN,
        SMASHABLE,
    };
}  // namespace ObjectGroupPhysicalProperties
