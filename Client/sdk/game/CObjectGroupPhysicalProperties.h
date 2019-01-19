/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CObjectGroupPhysicalProperties.h
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
    virtual void                                             RestoreDefault() = 0;

    virtual void                            SetMass(float fMass) = 0;
    virtual float                           GetMass() = 0;
    virtual void                            SetTurnMass(float fTurnMass) = 0;
    virtual float                           GetTurnMass() = 0;
    virtual void                            SetAirResistance(float fAirResistance) = 0;
    virtual float                           GetAirResistance() = 0;
    virtual void                            SetElasticity(float fElasticity) = 0;
    virtual float                           GetElasticity() = 0;
    virtual void                            SetBuoyancy(float fBuoyancy) = 0;
    virtual float                           GetBuoyancy() = 0;
    virtual void                            SetUprootLimit(float fUprootLimit) = 0;
    virtual float                           GetUprootLimit() = 0;
    virtual void                            SetCollisionDamageMultiplier(float fColMult) = 0;
    virtual float                           GetCollisionDamageMultiplier() = 0;
    virtual void                            SetCollisionDamageEffect(eObjectGroup::DamageEffect eDamageEffect) = 0;
    virtual eObjectGroup::DamageEffect      GetCollisionDamageEffect() = 0;
    virtual void                            SetCollisionSpecialResponseCase(eObjectGroup::CollisionResponse eResponseCase) = 0;
    virtual eObjectGroup::CollisionResponse GetCollisionSpecialResponseCase() = 0;
    virtual void                            SetCameraAvoidObject(bool bAvoid) = 0;
    virtual bool                            GetCameraAvoidObject() = 0;
    virtual void                            SetCausesExplosion(bool bExplodes) = 0;
    virtual bool                            GetCausesExplosion() = 0;
    virtual void                            SetFxType(eObjectGroup::FxType eFxType) = 0;
    virtual eObjectGroup::FxType            GetFxType() = 0;
    virtual void                            SetFxOffset(CVector vecOffset) = 0;
    virtual CVector                         GetFxOffset() = 0;
    virtual bool                            SetFxParticleSystem(CFxSystemBPSAInterface* pBlueprint) = 0;
    virtual void                            SetSmashMultiplier(float fMult) = 0;
    virtual float                           GetSmashMultiplier() = 0;
    virtual void                            SetBreakVelocity(CVector vecVelocity) = 0;
    virtual CVector                         GetBreakVelocity() = 0;
    virtual void                            SetBreakVelocityRandomness(float fRand) = 0;
    virtual float                           GetBreakVelocityRandomness() = 0;
    virtual void                            SetBreakMode(eObjectGroup::BreakMode eBreakMode) = 0;
    virtual eObjectGroup::BreakMode         GetBreakMode() = 0;
    virtual void                            SetSparksOnImpact(bool bSparks) = 0;
    virtual bool                            GetSparksOnImpact() = 0;
};
