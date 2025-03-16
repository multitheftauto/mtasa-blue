/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectSA.h
 *  PURPOSE:     Header file for object entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CObject.h>
#include "CPhysicalSA.h"

class CFireSAInterface;

#define FUNC_CObject_Create             0x5A1F60
#define FUNC_CObject_Explode            0x5A1340
#define FUNC_CGlass_WindowRespondsToCollision 0x71BC40

class CObjectInfo
{
public:
    float   fMass;                               // 0
    float   fTurnMass;                           // 4
    float   fAirResistance;                      // 8
    float   fElasticity;                         // 12
    float   fBuoyancy;                           // 16
    float   fUprootLimit;                        // 20
    float   fColDamageMultiplier;                // 24
    uint8   ucColDamageEffect;                   // 28
    uint8   ucSpecialColResponseCase;            // 29
    uint8   ucCameraAvoidObject;                 // 30
    uint8   ucCausesExplosion;                   // 31
    uint8   ucFxType;                            // 32
    uint8   pad1[3];                             // 33
    CVector vecFxOffset;                         // 36
    void*   pFxSystem;                           // ret from CParticleData::GetDataFromName // 48
    float   fSmashMultiplier;                    // 52
    CVector vecBreakVelocity;                    // 56
    float   fBreakVelocityRand;                  // 68
    uint32  uiGunBreakMode;                      // 72
    uint32  uiSparksOnImpact;                    // 76
};
// TODO: Find out correct size
// static_assert(sizeof(CObjectInfo) == 0x50, "Invalid size for CObjectInfo");

class CObjectSAInterface : public CPhysicalSAInterface
{
public:
    void*  pObjectList;            // 312
    uint8  pad1;                   // 316
    uint8  pad2;                   // 317
    uint16 pad3;                   // 318

    // flags
    uint32 b0x01 : 1;            // 320
    uint32 b0x02 : 1;
    uint32 b0x04 : 1;
    uint32 b0x08 : 1;
    uint32 b0x10 : 1;
    uint32 b0x20 : 1;
    uint32 bExploded : 1;
    uint32 b0x80 : 1;

    uint32 b0x100 : 1;            // 321
    uint32 b0x200 : 1;
    uint32 b0x400 : 1;
    uint32 bIsTrainNearCrossing : 1;            // Train crossing will be opened if flag is set (distance < 120.0f)
    uint32 b0x1000 : 1;
    uint32 b0x2000 : 1;
    uint32 bIsDoorMoving : 1;
    uint32 bIsDoorOpen : 1;

    uint32 b0x10000 : 1;            // 322
    uint32 bUpdateScale : 1;
    uint32 b0x40000 : 1;
    uint32 b0x80000 : 1;
    uint32 b0x100000 : 1;
    uint32 b0x200000 : 1;
    uint32 b0x400000 : 1;
    uint32 b0x800000 : 1;

    uint32 b0x1000000 : 1;            // 323
    uint32 b0x2000000 : 1;
    uint32 b0x4000000 : 1;
    uint32 b0x8000000 : 1;
    uint32 b0x10000000 : 1;
    uint32 b0x20000000 : 1;
    uint32 b0x40000000 : 1;
    uint32 b0x80000000 : 1;

    uint8               ucColDamageEffect;              // 324
    uint8               pad4;                           // 325
    uint8               pad5;                           // 326
    uint8               pad6;                           // 327
    uint8               pad7;                           // 328
    uint8               pad8;                           // 329
    uint16              pad9;                           // 330
    uint8               pad10;                          // 332
    uint8               pad11;                          // 333
    uint8               pad12;                          // 334
    uint8               pad13;                          // 335
    uint32              uiObjectRemovalTime;            // 336
    float               fHealth;                        // 340
    uint32              pad15;                          // 344
    float               fScale;                         // 348
    CObjectInfo*        pObjectInfo;                    // 352
    CFireSAInterface*   pFire;                          // 356
    uint16              pad17;                          // 360
    uint16              pad18;                          // 362
    uint32              pad19;                          // 364
    CEntitySAInterface* pLinkedObjectDummy;             // 368  CDummyObject - Is used for dynamic objects like garage doors, train crossings etc.
    uint32              pad21;                          // 372
    uint32              pad22;                          // 376
};
static_assert(sizeof(CObjectSAInterface) == 0x17C, "Invalid size for CObjectSAInterface");

class CObjectSA : public virtual CObject, public virtual CPhysicalSA
{
private:
    unsigned char m_ucAlpha;
    bool          m_bIsAGangTag;
    CVector       m_vecScale;
    bool          m_preRenderRequired = false;

public:
    static void StaticSetHooks();

    CObjectSA(CObjectSAInterface* objectInterface);
    CObjectSA(DWORD dwModel, bool bBreakingDisabled);
    ~CObjectSA();

    CObjectSAInterface* GetObjectInterface() { return (CObjectSAInterface*)GetInterface(); }

    void  Explode();
    void  Break();
    void  SetHealth(float fHealth);
    float GetHealth();
    void  SetModelIndex(unsigned long ulModel);

    void          SetPreRenderRequired(bool required) { m_preRenderRequired = required; }
    bool          GetPreRenderRequired() { return m_preRenderRequired; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }
    unsigned char GetAlpha() { return m_ucAlpha; }

    bool IsAGangTag() const { return m_bIsAGangTag; }
    bool IsGlass();

    void     SetScale(float fX, float fY, float fZ);
    CVector* GetScale();
    void     ResetScale();

    bool IsOnFire() override { return GetObjectInterface()->pFire != nullptr; }
    bool SetOnFire(bool onFire) override;

private:
    void CheckForGangTag();
};
