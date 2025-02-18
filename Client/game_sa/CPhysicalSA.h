/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPhysicalSA.h
 *  PURPOSE:     Header file for physical object entity base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPhysical.h>
#include "CEntitySA.h"
#include <CVector.h>
#include "CPtrNodeDoubleListSA.h"

#define FUNC_GetMoveSpeed                       0x404460
#define FUNC_GetTurnSpeed                       0x470030
#define FUNC_ProcessCollision                   0x54DFB0
#define FUNC_AttachEntityToEntity               0x54D570
#define FUNC_DetatchEntityFromEntity            0x5442F0
#define FUNC_CPhysical_AddToMovingList          0x542800
#define FUNC_CPhysical_RemoveFromMovingList     0x542860

#define PHYSICAL_MAXNOOFCOLLISIONRECORDS        6
#define PHYSICAL_MAXMASS                        99999.0

class CPhysicalSAInterface : public CEntitySAInterface
{
public:
    float  pad1;            // 56
    uint32 pad2;            // 60

    uint32 b0x01 : 1;            // 64
    uint32 bApplyGravity : 1;
    uint32 bDisableFriction : 1;
    uint32 bCollidable : 1;
    uint32 b0x10 : 1;
    uint32 bDisableMovement : 1;
    uint32 b0x40 : 1;
    uint32 b0x80 : 1;

    uint32 bSubmergedInWater : 1;            // 65
    uint32 bOnSolidSurface : 1;
    uint32 bBroken : 1;
    uint32 b0x800 : 1;                     // ref @ 0x6F5CF0
    uint32 b0x1000 : 1;                    //
    uint32 bDontApplySpeed : 1;            //
    uint32 b0x4000 : 1;                    //
    uint32 b0x8000 : 1;                    //

    uint32 b0x10000 : 1;            // 66
    uint32 b0x20000 : 1;            // ref @ CPhysical__processCollision
    uint32 bBulletProof : 1;
    uint32 bFireProof : 1;
    uint32 bCollisionProof : 1;
    uint32 bMeeleProof : 1;
    uint32 bInvulnerable : 1;
    uint32 bExplosionProof : 1;

    uint32 b0x1000000 : 1;            // 67
    uint32 bAttachedToEntity : 1;
    uint32 b0x4000000 : 1;
    uint32 bTouchingWater : 1;
    uint32 bEnableCollision : 1;
    uint32 bDestroyed : 1;
    uint32 b0x40000000 : 1;
    uint32 b0x80000000 : 1;

    CVector                   m_vecLinearVelocity;                        // 68
    CVector                   m_vecAngularVelocity;                       // 80
    CVector                   m_vecCollisionLinearVelocity;               // 92
    CVector                   m_vecCollisionAngularVelocity;              // 104
    CVector                   m_vecOffsetUnk5;                            // 116
    CVector                   m_vecOffsetUnk6;                            // 128
    float                     m_fMass;                                    // 140
    float                     m_fTurnMass;                                // 144
    float                     m_pad1;                                     // 148
    float                     m_fAirResistance;                           // 152
    float                     m_fElasticity;                              // 156
    float                     m_fBuoyancyConstant;                        // 160
    CVector                   m_vecCenterOfMass;                          // 164
    uint32*                   m_pCollisionList;                           // 176
    uint32*                   m_pMovingList;                              // 180
    uint8                     m_ucColFlag1;                               // 184
    uint8                     m_ucCollisionState;                         // 185
    uint8                     m_ucCollisionContactSurfaceType;            // 186
    uint8                     m_ucColFlag4;                               // 187
    CEntity*                  pLastContactedEntity[4];                    // 188
    float                     m_field_cc;                                 // 204
    float                     m_pad4c;                                    // 208
    float                     m_pad4d;                                    // 212
    float                     m_fDamageImpulseMagnitude;                  // 216
    CEntitySAInterface*       m_pCollidedEntity;                          // 220
    CVector                   m_vecCollisionImpactVelocity;               // 224
    CVector                   m_vecCollisionPosition;                     // 236
    uint16                    m_usPieceType;                              // 248
    uint16                    m_pad3;                                     // 250
    CEntitySAInterface*       m_pAttachedEntity;                          // 252
    CVector                   m_vecAttachedOffset;                        // 256
    CVector                   m_vecAttachedRotation;                      // 268
    CVector                   m_vecUnk;                                   // 280
    uint32                    m_pad4;                                     // 292
    CPtrNodeDoubleLink<void>* m_pControlCodeNodeLink;                           // 296
    float                     m_fLighting;                                // 300 surface brightness
    float                     m_fLighting2;                               // 304 dynamic lighting (unused, always set to 0 in the GTA code)
    class CShadowDataSA*      m_pShadowData;                              // 308

    CRect*      GetBoundRect_(CRect* pRect);
    static void StaticSetHooks();
};
static_assert(sizeof(CPhysicalSAInterface) == 0x138, "Invalid size for CPhysicalSAInterface");

class CPhysicalSA : public virtual CPhysical, public virtual CEntitySA
{
public:
    virtual void RestoreLastGoodPhysicsState();
    CVector*     GetMoveSpeed(CVector* vecMoveSpeed);
    CVector*     GetTurnSpeed(CVector* vecTurnSpeed);
    CVector*     GetMoveSpeedInternal(CVector* vecMoveSpeed);
    CVector*     GetTurnSpeedInternal(CVector* vecTurnSpeed);
    void         SetMoveSpeed(const CVector& vecMoveSpeed) noexcept;
    void         SetTurnSpeed(CVector* vecTurnSpeed);

    float GetMass();
    void  SetMass(float fMass);
    float GetTurnMass();
    void  SetTurnMass(float fTurnMass);
    float GetAirResistance();
    void  SetAirResistance(float fAirResistance);
    float GetElasticity();
    void  SetElasticity(float fElasticity);
    float GetBuoyancyConstant();
    void  SetBuoyancyConstant(float fBuoyancyConstant);
    void  GetCenterOfMass(CVector& vecCenterOfMass);
    void  SetCenterOfMass(CVector& vecCenterOfMass);

    void ProcessCollision();
    void AddToMovingList();

    float    GetDamageImpulseMagnitude();
    void     SetDamageImpulseMagnitude(float fMagnitude);
    CEntity* GetDamageEntity();
    void     SetDamageEntity(CEntity* pEntity);
    void     ResetLastDamage();

    CEntity* GetAttachedEntity();
    void     AttachEntityToEntity(CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation);
    void     DetachEntityFromEntity(float fUnkX, float fUnkY, float fUnkZ, bool bUnk);
    void     GetAttachedOffsets(CVector& vecPosition, CVector& vecRotation);
    void     SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation);

    virtual bool InternalAttachEntityToEntity(DWORD dwEntityInterface, const CVector* vecPosition, const CVector* vecRotation);

    float GetLighting();
    void  SetLighting(float fLighting);

    void SetFrozen(bool bFrozen);
};
