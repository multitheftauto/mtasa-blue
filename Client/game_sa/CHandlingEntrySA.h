/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle handling data entry class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHandlingEntry.h>

#define FUNC_HandlingDataMgr_ConvertDataToGameUnits 0x6F5080

// http://www.gtamodding.com/index.php?title=Handling.cfg#GTA_San_Andreas
// http://www.gtamodding.com/index.php?title=Memory_Addresses_%28SA%29#Handling

class CTransmissionSAInterface
{
public:
    float fUnknown[18];            // +40

    unsigned char ucDriveType : 8;                // +112
    unsigned char ucEngineType : 8;               // +113
    unsigned char ucNumberOfGears : 8;            // +114
    unsigned char ucUnknown : 8;                  // +115

    unsigned int uiHandlingFlags;            // +116

    float fEngineAcceleration;            // +120     (value in handling.cfg * 0x86A950)
    float fEngineInertia;                 // +124
    float fMaxVelocity;                   // +128

    float fUnknown2[3];            // +132
};

struct tHandlingDataSA
{
    int iVehicleID;            // +0

    float fMass;            // +4

    float fUnknown1;            // +8    Automatically calculated

    float        fTurnMass;                     // +12
    float        fDragCoeff;                    // +16
    CVector      vecCenterOfMass;               // +20
    unsigned int uiPercentSubmerged;            // +32

    float fUnknown2;            // +36  Automatically calculated

    float fTractionMultiplier;            // +40

    CTransmissionSAInterface Transmission;                  // +44
    float                    fBrakeDeceleration;            // +148
    float                    fBrakeBias;                    // +152
    bool                     bABS;                          // +156
    char                     fUnknown[3];                   // +157

    float fSteeringLock;            // +160
    float fTractionLoss;            // +164
    float fTractionBias;            // +168

    float fSuspensionForceLevel;                    // +172
    float fSuspensionDamping;                       // +176
    float fSuspensionHighSpdDamping;                // +180
    float fSuspensionUpperLimit;                    // +184
    float fSuspensionLowerLimit;                    // +188
    float fSuspensionFrontRearBias;                 // +192
    float fSuspensionAntiDiveMultiplier;            // +196

    float fCollisionDamageMultiplier;            // +200

    unsigned int uiModelFlags;                   // +204
    unsigned int uiHandlingFlags;                // +208
    float        fSeatOffsetDistance;            // +212
    unsigned int uiMonetary;                     // +216

    unsigned char ucHeadLight : 8;            // +220
    unsigned char ucTailLight : 8;            // +221
    unsigned char ucAnimGroup : 8;            // +222
};

class CHandlingEntrySA : public CHandlingEntry
{
public:
    // Constructor for creatable dummy entries
    CHandlingEntrySA();

    // Constructor for original entries
    CHandlingEntrySA(const tHandlingDataSA* const pOriginal);

    virtual ~CHandlingEntrySA(){};

    // Use this to copy data from an another handling class to this
    void Assign(const CHandlingEntry* const pEntry) noexcept;

    // Get functions
    float          GetMass() const noexcept { return m_Handling.fMass; }
    float          GetTurnMass() const noexcept { return m_Handling.fTurnMass; }
    float          GetDragCoeff() const noexcept { return m_Handling.fDragCoeff; }
    const CVector& GetCenterOfMass() const noexcept { return m_Handling.vecCenterOfMass; }

    unsigned int GetPercentSubmerged() const noexcept { return m_Handling.uiPercentSubmerged; }
    float        GetTractionMultiplier() const noexcept { return m_Handling.fTractionMultiplier; }

    eDriveType    GetCarDriveType() const noexcept { return static_cast<eDriveType>(m_Handling.Transmission.ucDriveType); }
    eEngineType   GetCarEngineType() const noexcept { return static_cast<eEngineType>(m_Handling.Transmission.ucEngineType); }
    unsigned char GetNumberOfGears() const noexcept { return m_Handling.Transmission.ucNumberOfGears; }

    float GetEngineAcceleration() const noexcept { return m_Handling.Transmission.fEngineAcceleration; }
    float GetEngineInertia() const noexcept { return m_Handling.Transmission.fEngineInertia; }
    float GetMaxVelocity() const noexcept { return m_Handling.Transmission.fMaxVelocity; }

    float GetBrakeDeceleration() const noexcept { return m_Handling.fBrakeDeceleration; }
    float GetBrakeBias() const noexcept { return m_Handling.fBrakeBias; }
    bool  GetABS() const noexcept { return m_Handling.bABS; }

    float GetSteeringLock() const noexcept { return m_Handling.fSteeringLock; }
    float GetTractionLoss() const noexcept { return m_Handling.fTractionLoss; }
    float GetTractionBias() const noexcept { return m_Handling.fTractionBias; }

    float GetSuspensionForceLevel() const noexcept { return m_Handling.fSuspensionForceLevel; }
    float GetSuspensionDamping() const noexcept { return m_Handling.fSuspensionDamping; }
    float GetSuspensionHighSpeedDamping() const noexcept { return m_Handling.fSuspensionHighSpdDamping; }
    float GetSuspensionUpperLimit() const noexcept { return m_Handling.fSuspensionUpperLimit; }
    float GetSuspensionLowerLimit() const noexcept { return m_Handling.fSuspensionLowerLimit; }
    float GetSuspensionFrontRearBias() const noexcept { return m_Handling.fSuspensionFrontRearBias; }
    float GetSuspensionAntiDiveMultiplier() const noexcept { return m_Handling.fSuspensionAntiDiveMultiplier; }

    float GetCollisionDamageMultiplier() const noexcept { return m_Handling.fCollisionDamageMultiplier; }

    unsigned int GetHandlingFlags() const noexcept { return m_Handling.uiHandlingFlags; }
    unsigned int GetModelFlags() const noexcept { return m_Handling.uiModelFlags; }
    float        GetSeatOffsetDistance() const noexcept { return m_Handling.fSeatOffsetDistance; }
    unsigned int GetMonetary() const noexcept { return m_Handling.uiMonetary; }

    eLightType    GetHeadLight() const noexcept { return static_cast<eLightType>(m_Handling.ucHeadLight); }
    eLightType    GetTailLight() const noexcept { return static_cast<eLightType>(m_Handling.ucTailLight); }
    unsigned char GetAnimGroup() const noexcept { return m_Handling.ucAnimGroup; }

    eHandlingTypes GetVehicleID() const noexcept { return static_cast<eHandlingTypes>(m_Handling.iVehicleID); }

    // Set functions
    void SetMass(float fMass) noexcept { m_Handling.fMass = fMass; }
    void SetTurnMass(float fTurnMass) noexcept { m_Handling.fTurnMass = fTurnMass; }
    void SetDragCoeff(float fDrag) noexcept { m_Handling.fDragCoeff = fDrag; }
    void SetCenterOfMass(const CVector& vecCenter) noexcept { m_Handling.vecCenterOfMass = vecCenter; }

    void SetPercentSubmerged(unsigned int uiPercent) noexcept { m_Handling.uiPercentSubmerged = uiPercent; }
    void SetTractionMultiplier(float fTractionMultiplier) noexcept { m_Handling.fTractionMultiplier = fTractionMultiplier; }

    void SetCarDriveType(eDriveType Type) noexcept { m_Handling.Transmission.ucDriveType = Type; }
    void SetCarEngineType(eEngineType Type) noexcept { m_Handling.Transmission.ucEngineType = Type; }
    void SetNumberOfGears(unsigned char ucNumber) noexcept { m_Handling.Transmission.ucNumberOfGears = ucNumber; }

    void SetEngineAcceleration(float fAcceleration) noexcept { m_Handling.Transmission.fEngineAcceleration = fAcceleration; }
    void SetEngineInertia(float fInertia) noexcept { m_Handling.Transmission.fEngineInertia = fInertia; }
    void SetMaxVelocity(float fVelocity) noexcept { m_Handling.Transmission.fMaxVelocity = fVelocity; }

    void SetBrakeDeceleration(float fDeceleration) noexcept { m_Handling.fBrakeDeceleration = fDeceleration; }
    void SetBrakeBias(float fBias) noexcept { m_Handling.fBrakeBias = fBias; }
    void SetABS(bool bABS) noexcept { m_Handling.bABS = bABS; }

    void SetSteeringLock(float fSteeringLock) noexcept { m_Handling.fSteeringLock = fSteeringLock; }
    void SetTractionLoss(float fTractionLoss) noexcept { m_Handling.fTractionLoss = fTractionLoss; }
    void SetTractionBias(float fTractionBias) noexcept { m_Handling.fTractionBias = fTractionBias; }

    void SetSuspensionForceLevel(float fForce) noexcept;
    void SetSuspensionDamping(float fDamping) noexcept;
    void SetSuspensionHighSpeedDamping(float fDamping) noexcept;
    void SetSuspensionUpperLimit(float fUpperLimit) noexcept;
    void SetSuspensionLowerLimit(float fLowerLimit) noexcept;
    void SetSuspensionFrontRearBias(float fBias) noexcept;
    void SetSuspensionAntiDiveMultiplier(float fAntidive) noexcept;

    void SetCollisionDamageMultiplier(float fMultiplier) noexcept { m_Handling.fCollisionDamageMultiplier = fMultiplier; }

    void SetHandlingFlags(unsigned int uiFlags) noexcept { m_Handling.uiHandlingFlags = uiFlags; }
    void SetModelFlags(unsigned int uiFlags) noexcept { m_Handling.uiModelFlags = uiFlags; }
    void SetSeatOffsetDistance(float fDistance) noexcept { m_Handling.fSeatOffsetDistance = fDistance; }
    void SetMonetary(unsigned int uiMonetary) noexcept { m_Handling.uiMonetary = uiMonetary; }

    void SetHeadLight(eLightType Style) noexcept { m_Handling.ucHeadLight = Style; }
    void SetTailLight(eLightType Style) noexcept { m_Handling.ucTailLight = Style; }
    void SetAnimGroup(unsigned char ucGroup) noexcept { m_Handling.ucAnimGroup = ucGroup; }

    void CheckSuspensionChanges() const noexcept;

    void Recalculate() noexcept;

    tHandlingDataSA* GetInterface() const noexcept { return m_HandlingSA.get(); }

private:
    std::unique_ptr<tHandlingDataSA> m_HandlingSA;
    tHandlingDataSA                  m_Handling;
};
