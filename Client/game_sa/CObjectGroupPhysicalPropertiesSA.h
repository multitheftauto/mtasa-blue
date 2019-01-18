/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectGroupPhysicalPropertiesSA.h
 *  PURPOSE:     Header file for objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CObjectGroupPhysicalProperties.h>
#include <game/Common.h>

#define ARRAY_ObjectGroupsDynamicInfo 0xBB4A90

class CObjectGroupPhysicalPropertiesSAInterface
{
public:
    float                           fMass;
    float                           fTurnMass;
    float                           fAirResistance;
    float                           fElasticity;
    float                           fBuoyancy;
    float                           fUprootLimit;
    float                           fColDamageMultiplier;
    eObjectGroup::DamageEffect      eColDamageEffect;
    eObjectGroup::CollisionResponse eSpecialColResponse;
    bool                            bCameraAvoidObject;
    bool                            bCausesExplosion;
    eObjectGroup::FxType            eFxType;
    unsigned char                   pad[3];
    CVector                         vecFxOffset;
    void*                           pFxSystemPtr;
    float                           fSmashMultiplier;
    CVector                         vecBreakVelocity;
    float                           fBreakVelocityRand;
    eObjectGroup::BreakMode         eBreakMode;
    DWORD                           dwSparksOnImpact;
};

class CObjectGroupPhysicalPropertiesSA : public CObjectGroupPhysicalProperties
{
protected:
    CObjectGroupPhysicalPropertiesSAInterface*                                           m_pInterface;
    unsigned char                                                                        m_ucObjectGroup;
    bool                                                                                 m_bModified;
    static std::unordered_map<unsigned char, CObjectGroupPhysicalPropertiesSAInterface*> ms_OriginalGroupProperties;

public:
    CObjectGroupPhysicalPropertiesSA();
    CObjectGroupPhysicalPropertiesSA(unsigned char ucObjectGroup);

    CObjectGroupPhysicalPropertiesSAInterface* GetInterface();
    void                                       SetGroup(unsigned char ucObjectGroup);
    unsigned char                              GetGroup();
    bool                                       IsValid();
    void                                       ChangeSafeguard();
    void                                       RestoreDefault();
    static void                                RestoreDefaultValues();

    void                            SetMass(float fMass);
    float                           GetMass();
    void                            SetTurnMass(float fTurnMass);
    float                           GetTurnMass();
    void                            SetAirResistance(float fAirResistance);
    float                           GetAirResistance();
    void                            SetElasticity(float fElasticity);
    float                           GetElasticity();
    void                            SetBuoyancy(float fBuoyancy);
    float                           GetBuoyancy() override;
    void                            SetUprootLimit(float fUprootLimit);
    float                           GetUprootLimit();
    void                            SetColissionDamageMultiplier(float fColMult);
    float                           GetColissionDamageMultiplier();
    void                            SetColissionDamageEffect(eObjectGroup::DamageEffect ucDamageEffect);
    eObjectGroup::DamageEffect      GetCollisionDamageEffect();
    void                            SetCollisionSpecialResponseCase(eObjectGroup::CollisionResponse ucResponseCase);
    eObjectGroup::CollisionResponse GetCollisionSpecialResponseCase();
    void                            SetCameraAvoidObject(bool bAvoid);
    bool                            GetCameraAvoidObject();
    void                            SetCausesExplosion(bool bExplodes);
    bool                            GetCausesExplosion();
    void                            SetFxType(eObjectGroup::FxType eFxType);
    eObjectGroup::FxType            GetFxType();
    void                            SetFxOffset(CVector vecOffset);
    CVector                         GetFxOffset();
    void                            SetSmashMultiplier(float fMult);
    float                           GetSmashMultiplier();
    void                            SetBreakVelocity(CVector vecVelocity);
    CVector                         GetBreakVelocity();
    void                            SetBreakVelocityRandomness(float fRand);
    float                           GetBreakVelocityRandomness();
    void                            SetBreakMode(eObjectGroup::BreakMode eBreakMode);
    eObjectGroup::BreakMode         GetBreakMode();
    void                            SetSparksOnImpact(bool bSparks);
    bool                            GetSparksOnImpact();
};
