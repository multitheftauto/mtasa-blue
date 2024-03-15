/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
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
    float                   fMass;
    float                   fTurnMass;
    float                   fAirResistance;
    float                   fElasticity;
    float                   fBuoyancy;
    float                   fUprootLimit;
    float                   fColDamageMultiplier;
    std::uint8_t           eColDamageEffect;
    std::uint8_t           eSpecialColResponse;
    bool                    bCameraAvoidObject;
    bool                    bCausesExplosion;
    std::uint8_t           eFxType;
    std::uint8_t           pad[3];
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
    std::uint8_t                                                                                        m_ucObjectGroup;
    bool                                                                                                 m_bModified;
    static std::unordered_map<std::uint8_t, std::unique_ptr<CObjectGroupPhysicalPropertiesSAInterface>> ms_OriginalGroupProperties;

public:
    CObjectGroupPhysicalPropertiesSA();
    CObjectGroupPhysicalPropertiesSA(std::uint8_t ucObjectGroup);

    CObjectGroupPhysicalPropertiesSAInterface* GetInterface() const;
    void                                       SetGroup(std::uint8_t ucObjectGroup);
    std::uint8_t                              GetGroup() const;
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
    void                            SetCollisionDamageEffect(eObjectGroup::DamageEffect ucDamageEffect);
    eObjectGroup::DamageEffect      GetCollisionDamageEffect() const;
    void                            SetCollisionSpecialResponseCase(eObjectGroup::CollisionResponse ucResponseCase);
    eObjectGroup::CollisionResponse GetCollisionSpecialResponseCase() const;
    void                            SetCameraAvoidObject(bool bAvoid);
    bool                            GetCameraAvoidObject() const;
    void                            SetCausesExplosion(bool bExplodes);
    bool                            GetCausesExplosion() const;
    void                            SetFxType(eObjectGroup::FxType eFxType);
    eObjectGroup::FxType            GetFxType() const;
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
    void                            SetBreakMode(eObjectGroup::BreakMode eBreakMode);
    eObjectGroup::BreakMode         GetBreakMode() const;
    void                            SetSparksOnImpact(bool bSparks);
    bool                            GetSparksOnImpact() const;
};
