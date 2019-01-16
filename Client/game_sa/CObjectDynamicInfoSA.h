/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectDynamicInfoSA.h
 *  PURPOSE:     Header file for objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CObjectDynamicInfo.h>
#include "Common.h"

#define ARRAY_ObjectGroupsDynamicInfo 0xBB4A90

class CObjectDynamicInfoSAInterface
{
public:
    float         fMass;
    float         fTurnMass;
    float         fAirResistance;
    float         fElasticity;
    float         fBuoyancy;
    float         fUprootLimit;
    float         fColDamageMultiplier;
    unsigned char ucColDamageEffect;
    unsigned char ucSpecialColResponse;
    bool          bCameraAvoidObject;
    bool          bCausesExplosion;
    unsigned char ucFxType;
    unsigned char pad[3];
    CVector       vecFxOffset;
    DWORD         pad2;
    float         fSmashMultiplier;
    CVector       vecBreakVelocity;
    float         fBreakVelocityRand;
    DWORD         dwGunBreakMode;
    DWORD         dwSparksOnImpact;
};

class CObjectDynamicInfoSA : public CObjectDynamicInfo
{
protected:
    CObjectDynamicInfoSAInterface* m_pInterface;
    CObjectDynamicInfoSAInterface* m_pOriginalCopy;
    unsigned char                  m_ucObjectGroup;

public:
    CObjectDynamicInfoSA();
    CObjectDynamicInfoSA(unsigned char ucObjectGroup);
    ~CObjectDynamicInfoSA();

    CObjectDynamicInfoSAInterface* GetInterface();
    void                           SetObjectGroup(unsigned char ucObjectGroup);
    unsigned char                  GetObjectGroup();
    bool                           IsValid();

    void          SetMass(float fMass);
    float         GetMass();
    void          SetTurnMass(float fTurnMass);
    float         GetTurnMass();
    void          SetAirResistance(float fAirResistance);
    float         GetAirResistance();
    void          SetElasticity(float fElasticity);
    float         GetElasticity();
    void          SetUprootLimit(float fUprootLimit);
    float         GetUprootLimit();
    void          SetColissionDamageMultiplier(float fColMult);
    float         GetColissionDamageMultiplier();
    void          SetColissionDamageEffect(unsigned char ucDamageEffect);
    unsigned char GetCollisionDamageEffect();
    void          SetCollisionSpecialResponseCase(unsigned char ucResponseCase);
    unsigned char GetCollisionSpecialResponseCase();
    void          SetCameraAvoidObject(bool bAvoid);
    bool          GetCameraAvoidObject();
    void          SetCausesExplosion(bool bExplodes);
    bool          GetCausesExplosion();
    void          SetFxOffset(CVector vecOffset);
    CVector       GetFxOffset();
    void          SetSmashMultiplier(float fMult);
    float         GetSmashMultiplier();
    void          SetBreakVelocity(CVector vecVelocity);
    CVector       GetBreakVelocity();
    void          SetBreakVelocityRandomness(float fRand);
    float         GetBreakVelocityRandomness();
    void          SetGunBreakMode(DWORD dwBreakMode);
    DWORD         GetGunBreakMode();
    void          SetSparksOnImpact(DWORD dwSparks);
    DWORD         GetSparksOnImpact();
    void          RestoreDefaultValues();
};
