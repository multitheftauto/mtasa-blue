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

class CPhysicalSAInterface : public CEntitySAInterface
{
public:
    CVector    m_vecLinearVelocity; // 68
    CVector    m_vecAngularVelocity; // 80
    CVector    m_vecCollisionLinearVelocity; // 92
    CVector    m_vecCollisionAngularVelocity; // 104
    CVector    m_vecOffsetUnk5; // 116
    CVector    m_vecOffsetUnk6; // 128
    float m_fMass; // 140
    float m_fTurnMass; // 144
    float m_pad1; // 148
    float m_fAirResistance; // 152
    float m_fElasticity; // 156
    float m_fBuoyancyConstant; // 160
    CVector m_vecCenterOfMass; // 164
    uint32 * m_pCollisionList; // 176
    uint32 * m_pMovingList; // 180
    uint8 m_ucColFlag1; // 184
    uint8 m_ucCollisionState; // 185
    uint8 m_ucCollisionContactSurfaceType; // 186
    uint8 m_ucColFlag4; // 187
    CEntity* pLastContactedEntity[4]; // 188
    float m_field_cc; // 204
    float m_pad4c; // 208
    float m_pad4d; // 212
    float m_fDamageImpulseMagnitude; // 216
    CEntitySAInterface * m_pCollidedEntity; // 220
    CVector m_vecCollisionImpactVelocity; // 224
    CVector m_vecCollisionPosition; // 236
    uint16 m_usPieceType; // 248
    uint16 m_pad3; // 250
    CEntitySAInterface * m_pAttachedEntity;   // 252
    CVector m_vecAttachedOffset;    // 256
    CVector m_vecAttachedRotation;    // 268
    CVector m_vecUnk;    // 280
    uint32 m_pad4[2]; // 292
    float m_fLighting; // 300           // col lighting? CPhysical::GetLightingFromCol
    float m_fLighting2; // 304          // added to col lighting in CPhysical::GetTotalLighting
    DWORD *m_pShadowData; // 308
};
C_ASSERT(sizeof(CPhysicalSAInterface) == 0x138);

class CPhysicalSA : public virtual CPhysical, public virtual CEntitySA
{
public:
    virtual void RestoreLastGoodPhysicsState ( void );
    CVector *   GetMoveSpeed                ( CVector * vecMoveSpeed );
    CVector *   GetTurnSpeed                ( CVector * vecTurnSpeed );
    CVector *   GetMoveSpeedInternal        ( CVector * vecMoveSpeed );
    CVector *   GetTurnSpeedInternal        ( CVector * vecTurnSpeed );
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
