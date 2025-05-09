/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectGroupPhysicalPropertiesSA.h
 *  PURPOSE:     Header file for objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CObjectGroupPhysicalProperties.h>
#include <game/Common.h>

#define ARRAY_ObjectGroupsDynamicInfo 0xBB4A90

class CObjectGroupPhysicalPropertiesSAInterface
{
public:
    float                   fMass;
    float                   fTurnMass;
    float                   fAirResistance;
    float                   fElasticity;
    float                   fBuoyancy;
    float                   fUprootLimit;
    float                   fColDamageMultiplier;
    unsigned char           eColDamageEffect;
    unsigned char           eSpecialColResponse;
    bool                    bCameraAvoidObject;
    bool                    bCausesExplosion;
    unsigned char           eFxType;
    unsigned char           pad[3];
    CVector                 vecFxOffset;
    CFxSystemBPSAInterface* pFxSystemBlueprintPtr;
    float                   fSmashMultiplier;
    CVector                 vecBreakVelocity;
    float                   fBreakVelocityRand;
    DWORD                   eBreakMode;
    DWORD                   dwSparksOnImpact;
};

class CObjectGroupPhysicalPropertiesSA : public CObjectGroupPhysicalProperties
{
protected:
    CObjectGroupPhysicalPropertiesSAInterface*                                                           m_pInterface;
    unsigned char                                                                                        m_ucObjectGroup;
    bool                                                                                                 m_bModified;
    static std::unordered_map<unsigned char, std::unique_ptr<CObjectGroupPhysicalPropertiesSAInterface>> ms_OriginalGroupProperties;

public:
    CObjectGroupPhysicalPropertiesSA();
    CObjectGroupPhysicalPropertiesSA(unsigned char ucObjectGroup);

    CObjectGroupPhysicalPropertiesSAInterface* GetInterface() const;
    void                                       SetGroup(unsigned char ucObjectGroup);
    unsigned char                              GetGroup() const;
    bool                                       IsValid() const;
    void                                       ChangeSafeguard();
    void                                       RestoreDefault();
    static void                                RestoreDefaultValues();

    void                            SetMass(float fMass);
    float                           GetMass() const;
    void                            SetTurnMass(float fTurnMass);
    float                           GetTurnMass() const;
    void                            SetAirResistance(float fAirResistance);
    float                           GetAirResistance() const;
    void                            SetElasticity(float fElasticity);
    float                           GetElasticity() const;
    void                            SetBuoyancy(float fBuoyancy);
    float                           GetBuoyancy() const;
    void                            SetUprootLimit(float fUprootLimit);
    float                           GetUprootLimit() const;
    void                            SetCollisionDamageMultiplier(float fColMult);
    float                           GetCollisionDamageMultiplier() const;
    void                            SetCollisionDamageEffect(ObjectGroupPhysicalProperties::DamageEffect ucDamageEffect);
    ObjectGroupPhysicalProperties::DamageEffect      GetCollisionDamageEffect() const;
    void                            SetCollisionSpecialResponseCase(ObjectGroupPhysicalProperties::CollisionResponse ucResponseCase);
    ObjectGroupPhysicalProperties::CollisionResponse GetCollisionSpecialResponseCase() const;
    void                            SetCameraAvoidObject(bool bAvoid);
    bool                            GetCameraAvoidObject() const;
    void                            SetCausesExplosion(bool bExplodes);
    bool                            GetCausesExplosion() const;
    void                            SetFxType(ObjectGroupPhysicalProperties::FxType eFxType);
    ObjectGroupPhysicalProperties::FxType            GetFxType() const;
    void                            SetFxOffset(CVector vecOffset);
    CVector                         GetFxOffset() const;
    bool                            SetFxParticleSystem(CFxSystemBPSAInterface* pBlueprint);
    void                            RemoveFxParticleSystem();
    void                            SetSmashMultiplier(float fMult);
    float                           GetSmashMultiplier() const;
    void                            SetBreakVelocity(CVector vecVelocity);
    CVector                         GetBreakVelocity() const;
    void                            SetBreakVelocityRandomness(float fRand);
    float                           GetBreakVelocityRandomness() const;
    void                            SetBreakMode(ObjectGroupPhysicalProperties::BreakMode eBreakMode);
    ObjectGroupPhysicalProperties::BreakMode         GetBreakMode() const;
    void                            SetSparksOnImpact(bool bSparks);
    bool                            GetSparksOnImpact() const;
};
