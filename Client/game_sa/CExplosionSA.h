/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CExplosionSA.h
 *  PURPOSE:     Header file for explosion class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CExplosion.h>

class CEntitySAInterface;

class CExplosionSAInterface            // 124 bytes, ok
{
public:
    eExplosionType      m_ExplosionType;
    CVector             m_vecPosition;
    float               m_fExplosionRadius;
    float               m_fExplosionPropagationRate;
    CEntitySAInterface* m_pEntExplosionOwner;
    CEntitySAInterface* m_pExplodingEntity;            // 28
    DWORD               m_TimeExpires;
    float               m_DamagePercentage;
    BYTE                m_cExplosionActive;            // 36
    BYTE                m_nTriggerExplosionSfx;
    BYTE                m_bMakeSound;
    float               m_ParticleTimer;              // 40
    DWORD               m_ActivationTime;             // 44
    float               m_fExplosionForce;            // 48
    float               m_fGroundZ;
    DWORD               m_fuelTime;
    CVector             m_fuelDir[3];
    float               m_fuelOffsetDist[3];
    float               m_fuelSpeed[3];
};

class CExplosionSA : public CExplosion
{
private:
    CExplosionSAInterface* internalInterface;

public:
    CExplosionSA(CExplosionSAInterface* explosionInterface) { internalInterface = explosionInterface; }

    CExplosionSAInterface* GetInterface() { return internalInterface; };

    eExplosionType GetExplosionType();
    CVector*       GetExplosionPosition();
    void           SetExplosionPosition(const CVector* vecPosition);
    CEntity*       GetExplosionCreator();
    CEntity*       GetExplodingEntity();
    bool           IsActive();
    void           Remove();
    float          GetExplosionForce();
    void           SetExplosionForce(float fForce);
    void           SetSilent(bool bSilent);
    unsigned long  GetActivationTimer();
    void           SetActivationTimer(unsigned long ulActivationTime);
    DWORD          GetExpiryTime();
    void           SetExpiryTime(DWORD dwExpiryTime);
    float          GetExplosionRadius();
    void           SetExplosionRadius(float fRadius);
};
