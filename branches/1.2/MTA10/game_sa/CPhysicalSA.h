/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPhysicalSA.h
*  PURPOSE:     Header file for physical object entity base class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PHYSICAL
#define __CGAMESA_PHYSICAL

#include <game/CPhysical.h>
#include "CEntitySA.h"
#include <CVector.h>

#define FUNC_GetMoveSpeed                       0x404460
#define FUNC_GetTurnSpeed                       0x470030
#define FUNC_ProcessCollision                   0x54DFB0
#define FUNC_AttachEntityToEntity               0x54D570
#define FUNC_DetatchEntityFromEntity            0x5442F0

#define PHYSICAL_MAXNOOFCOLLISIONRECORDS        6

class CPhysicalSAInterface : public CEntitySAInterface // begin +68 (244 bytes total?)
{
public:
/* IMPORTANT: KEEP "pad" in CVehicle UP-TO-DATE if you add something here (or eventually pad someplace else) */
    CVector * vecVelocity;
    CVector * vecSpin;
    CVector * vecUnk3;
    CVector * vecUnk4;
    CVector * vecUnk5;
    CVector * vecUnk6;
    float pad [12];
    float fMass;
    float fTurnMass;

    DWORD dwUnk; // 148
    DWORD dwPhysUnk1; // 152

    float fElasticity; // 156
    float fBuoyancyConstant; // 160
    CVector * vecCenterOfMass; // 164

    DWORD dwUnk2; // 176
    DWORD * unkCPtrNodeDoubleLink; // 180

    BYTE byUnk: 8; 
    BYTE byCollisionRecords: 8; // 185
    BYTE byUnk2: 8;
    BYTE byUnk3: 8;

    float pad4 [8];

    float fDistanceTravelled; // 212
    float fDamageImpulseMagnitude; //216
    CEntitySAInterface * damageEntity; // 220

    BYTE pad2[28];
    CEntitySAInterface * pAttachedEntity;   // 252
    CVector vecAttachedPosition;    // 256
    CVector vecAttachedRotation;    // 268
    BYTE pad3[20];  // 280
    float fLighting;        // col lighting? CPhysical::GetLightingFromCol
    float fLighting_2;      // added to col lighting in CPhysical::GetTotalLighting
    BYTE pad3a[4];
};

class CPhysicalSA : public virtual CPhysical, public virtual CEntitySA
{
public:
    CVector *   GetMoveSpeed                ( CVector * vecMoveSpeed );
    CVector *   GetTurnSpeed                ( CVector * vecTurnSpeed );
    VOID        SetMoveSpeed                ( CVector * vecMoveSpeed );
    VOID        SetTurnSpeed                ( CVector * vecTurnSpeed );

    float       GetMass                     ( void );
    void        SetMass                     ( float fMass );
    float       GetTurnMass                 ( void );
    void        SetTurnMass                 ( float fTurnMass );
    float       GetElasticity               ( void );
    void        SetElasticity               ( float fElasticity );
    float       GetBuoyancyConstant         ( void );
    void        SetBuoyancyConstant         ( float fBuoyancyConstant );

    VOID        ProcessCollision            ( void );

    float       GetDamageImpulseMagnitude   ( void );
    void        SetDamageImpulseMagnitude   ( float fMagnitude );
    CEntity*    GetDamageEntity             ( void );
    void        SetDamageEntity             ( CEntity* pEntity );
    void        ResetLastDamage             ( void );

    CEntity *   GetAttachedEntity           ( void );
    void        AttachEntityToEntity        ( CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation );
    void        DetachEntityFromEntity      ( float fUnkX, float fUnkY, float fUnkZ, bool bUnk );
    void        GetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation );
    void        SetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation );

    virtual bool InternalAttachEntityToEntity ( DWORD dwEntityInterface, const CVector * vecPosition, const CVector * vecRotation );

    float       GetLighting                 ( void );
    void        SetLighting                 ( float fLighting );

    /*
    VOID        SetMassMultiplier(FLOAT fMassMultiplier);
    FLOAT       GetMassMultiplier();
    VOID        SetAirResistance(FLOAT fAirResistance);
    FLOAT       GetAirResistance();
    VOID        SetCenterOfMass(CVector * vecCenterOfMass);
    CVector * GetCenterOfMass();

    BOOL        GetExtraHeavy();
    VOID        SetExtraHeavy(BOOL bExtraHeavy);
    BOOL        GetDoGravity();
    VOID        SetDoGravity(BOOL bDoGravity);
    BOOL        GetInfiniteMass();
    VOID        SetInfiniteMass(BOOL bInfiniteMass);
    BOOL        GetPositionFrozen();
    VOID        SetPositionFrozen(BOOL bPositionFrozen);
    BYTE        GetLastMaterialToHaveBeenStandingOn();

    BYTE        GetLevel();
    VOID        SetLevel(BYTE LivesInThisLevel);*/

};

#endif
