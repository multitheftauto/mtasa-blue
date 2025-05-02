/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CExplosion.h
 *  PURPOSE:     Explosition interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEntity;
class CVector;

enum eExplosionType
{
    EXP_TYPE_GRENADE,
    EXP_TYPE_MOLOTOV,
    EXP_TYPE_ROCKET,
    EXP_TYPE_ROCKET_WEAK,
    EXP_TYPE_CAR,
    EXP_TYPE_CAR_QUICK,
    EXP_TYPE_BOAT,
    EXP_TYPE_HELI,
    EXP_TYPE_MINE,
    EXP_TYPE_OBJECT,
    EXP_TYPE_TANK_GRENADE,
    EXP_TYPE_SMALL,
    EXP_TYPE_TINY,
};

class CExplosion
{
public:
    virtual eExplosionType GetExplosionType() = 0;
    virtual CVector*       GetExplosionPosition() = 0;
    virtual void           SetExplosionPosition(const CVector* vecPosition) = 0;
    virtual CEntity*       GetExplosionCreator() = 0;
    virtual CEntity*       GetExplodingEntity() = 0;
    virtual float          GetExplosionForce() = 0;
    virtual void           SetExplosionForce(float fForce) = 0;
    virtual void           SetSilent(bool bSilent) = 0;
    virtual unsigned long  GetActivationTimer() = 0;
    virtual void           SetActivationTimer(unsigned long ulActivationTime) = 0;
    virtual DWORD          GetExpiryTime() = 0;
    virtual void           SetExpiryTime(DWORD dwExpiryTime) = 0;
    virtual float          GetExplosionRadius() = 0;
    virtual void           SetExplosionRadius(float fRadius) = 0;
};
