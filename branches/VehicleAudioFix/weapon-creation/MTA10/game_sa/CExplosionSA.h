/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExplosionSA.h
*  PURPOSE:     Header file for explosion class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EXPLOSION
#define __CGAMESA_EXPLOSION

#include <game/CExplosion.h>
#include "CEntitySA.h"

class CExplosionSAInterface // 124 bytes, ok
{
public:
    eExplosionType          m_ExplosionType;
    CVector                 m_vecPosition;
    FLOAT                   m_fExplosionRadius;
    FLOAT                   m_fExplosionPropagationRate;
    CEntitySAInterface      * m_pEntExplosionOwner;
    CEntitySAInterface      * m_pExplodingEntity; // 28
    DWORD                   m_TimeExpires;
    FLOAT                   m_DamagePercentage;
    BYTE                    m_cExplosionActive; // 36
    BYTE                    m_nTriggerExplosionSfx;
    BYTE                    m_bMakeSound;
    FLOAT                   m_ParticleTimer; // 40
    DWORD                   m_ActivationTime; // 44
    FLOAT                   m_fExplosionForce; // 48
    FLOAT                   m_fGroundZ;
    DWORD                   m_fuelTime;
    CVector                 m_fuelDir[3];
    FLOAT                   m_fuelOffsetDist[3];
    FLOAT                   m_fuelSpeed[3];
};

class CExplosionSA : public CExplosion
{
private:
    CExplosionSAInterface   * internalInterface;
public:
    CExplosionSA(CExplosionSAInterface * explosionInterface) { this->internalInterface = explosionInterface; }

    CExplosionSAInterface   * GetInterface() { return this->internalInterface; };

    eExplosionType          GetExplosionType            ( void );
    CVector*                GetExplosionPosition        ( void );
    void                    SetExplosionPosition        ( const CVector* vecPosition );
    CEntity*                GetExplosionCreator         ( void );
    CEntity*                GetExplodingEntity          ( void );
    BOOL                    IsActive                    ( void );
    VOID                    Remove                      ( void );
    float                   GetExplosionForce           ( void );
    void                    SetExplosionForce           ( float fForce );
    void                    SetSilent                   ( bool bSilent );
    unsigned long           GetActivationTimer          ( void );
    void                    SetActivationTimer          ( unsigned long ulActivationTime );
    DWORD                   GetExpiryTime               ( void );
    void                    SetExpiryTime               ( DWORD dwExpiryTime );
    float                   GetExplosionRadius          ( void );
    void                    SetExplosionRadius          ( float fRadius );
};

#endif
