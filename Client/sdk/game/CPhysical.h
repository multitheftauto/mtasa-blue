/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPhysical.h
 *  PURPOSE:     Physical entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEntity.h"

class CPhysical : public virtual CEntity
{
public:
    virtual ~CPhysical(){};

    virtual CVector* GetMoveSpeed(CVector* vecMoveSpeed) = 0;
    virtual CVector* GetTurnSpeed(CVector* vecTurnSpeed) = 0;
    virtual void     SetMoveSpeed(const CVector& vecMoveSpeed) noexcept = 0;
    virtual void     SetTurnSpeed(CVector* vecTurnSpeed) = 0;

    virtual float GetMass() = 0;
    virtual void  SetMass(float fMass) = 0;
    virtual float GetTurnMass() = 0;
    virtual void  SetTurnMass(float fTurnMass) = 0;
    virtual float GetAirResistance() = 0;
    virtual void  SetAirResistance(float fAirResistance) = 0;
    virtual float GetElasticity() = 0;
    virtual void  SetElasticity(float fElasticity) = 0;
    virtual float GetBuoyancyConstant() = 0;
    virtual void  SetBuoyancyConstant(float fBuoyancyConstant) = 0;
    virtual void  GetCenterOfMass(CVector& vecCenterOfMass) = 0;
    virtual void  SetCenterOfMass(CVector& vecCenterOfMass) = 0;

    virtual void ProcessCollision() = 0;
    virtual void AddToMovingList() = 0;

    virtual float    GetDamageImpulseMagnitude() = 0;
    virtual void     SetDamageImpulseMagnitude(float fMagnitude) = 0;
    virtual CEntity* GetDamageEntity() = 0;
    virtual void     SetDamageEntity(CEntity* pEntity) = 0;
    virtual void     ResetLastDamage() = 0;

    virtual CEntity* GetAttachedEntity() = 0;
    virtual void     AttachEntityToEntity(CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation) = 0;
    virtual void     DetachEntityFromEntity(float fUnkX, float fUnkY, float fUnkZ, bool bUnk) = 0;
    virtual void     GetAttachedOffsets(CVector& vecPosition, CVector& vecRotation) = 0;
    virtual void     SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation) = 0;

    virtual float GetLighting() = 0;
    virtual void  SetLighting(float fLighting) = 0;

    virtual void SetFrozen(bool bFrozen) = 0;
};
