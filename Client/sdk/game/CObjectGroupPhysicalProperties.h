/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CObjectDynamicInfo.h
 *  PURPOSE:     Objects dynamic physical properties handler interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <CVector.h>

class CObjectGroupPhysicalProperties
{
public:
    virtual class CObjectGroupPhysicalPropertiesSAInterface* GetInterface() = 0;
    virtual unsigned char                                    GetGroup() = 0;

    virtual void          SetMass(float fMass) = 0;
    virtual float         GetMass() = 0;
    virtual void          SetTurnMass(float fTurnMass) = 0;
    virtual float         GetTurnMass() = 0;
    virtual void          SetAirResistance(float fAirResistance) = 0;
    virtual float         GetAirResistance() = 0;
    virtual void          SetElasticity(float fElasticity) = 0;
    virtual float         GetElasticity() = 0;
    virtual void          SetUprootLimit(float fUprootLimit) = 0;
    virtual float         GetUprootLimit() = 0;
    virtual void          SetColissionDamageMultiplier(float fColMult) = 0;
    virtual float         GetColissionDamageMultiplier() = 0;
    virtual void          SetColissionDamageEffect(unsigned char ucDamageEffect) = 0;
    virtual unsigned char GetCollisionDamageEffect() = 0;
    virtual void          SetCollisionSpecialResponseCase(unsigned char ucResponseCase) = 0;
    virtual unsigned char GetCollisionSpecialResponseCase() = 0;
    virtual void          SetCameraAvoidObject(bool bAvoid) = 0;
    virtual bool          GetCameraAvoidObject() = 0;
    virtual void          SetCausesExplosion(bool bExplodes) = 0;
    virtual bool          GetCausesExplosion() = 0;
    virtual void          SetFxOffset(CVector vecOffset) = 0;
    virtual CVector       GetFxOffset() = 0;
    virtual void          SetSmashMultiplier(float fMult) = 0;
    virtual float         GetSmashMultiplier() = 0;
    virtual void          SetBreakVelocity(CVector vecVelocity) = 0;
    virtual CVector       GetBreakVelocity() = 0;
    virtual void          SetBreakVelocityRandomness(float fRand) = 0;
    virtual float         GetBreakVelocityRandomness() = 0;
    virtual void          SetGunBreakMode(DWORD dwBreakMode) = 0;
    virtual DWORD         GetGunBreakMode() = 0;
    virtual void          SetSparksOnImpact(DWORD dwSparks) = 0;
    virtual DWORD         GetSparksOnImpact() = 0;
};
