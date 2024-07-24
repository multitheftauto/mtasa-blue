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
    CHandlingEntrySA(tHandlingDataSA* pOriginal);

    virtual ~CHandlingEntrySA();

    // Use this to copy data from an another handling class to this
    void Assign(const CHandlingEntry* pEntry);

    // Get functions
    float          GetMass() const { return m_Handling.fMass; }
    float          GetTurnMass() const { return m_Handling.fTurnMass; }
    float          GetDragCoeff() const { return m_Handling.fDragCoeff; }
    const CVector& GetCenterOfMass() const { return m_Handling.vecCenterOfMass; }

    unsigned int GetPercentSubmerged() const { return m_Handling.uiPercentSubmerged; }
    float        GetTractionMultiplier() const { return m_Handling.fTractionMultiplier; }

    eDriveType    GetCarDriveType() const { return static_cast<eDriveType>(m_Handling.Transmission.ucDriveType); }
    eEngineType   GetCarEngineType() const { return static_cast<eEngineType>(m_Handling.Transmission.ucEngineType); }
    unsigned char GetNumberOfGears() const { return m_Handling.Transmission.ucNumberOfGears; }

    float GetEngineAcceleration() const { return m_Handling.Transmission.fEngineAcceleration; }
    float GetEngineInertia() const { return m_Handling.Transmission.fEngineInertia; }
    float GetMaxVelocity() const { return m_Handling.Transmission.fMaxVelocity; }

    float GetBrakeDeceleration() const { return m_Handling.fBrakeDeceleration; }
    float GetBrakeBias() const { return m_Handling.fBrakeBias; }
    bool  GetABS() const { return m_Handling.bABS; }

    float GetSteeringLock() const { return m_Handling.fSteeringLock; }
    float GetTractionLoss() const { return m_Handling.fTractionLoss; }
    float GetTractionBias() const { return m_Handling.fTractionBias; }

    float GetSuspensionForceLevel() const { return m_Handling.fSuspensionForceLevel; }
    float GetSuspensionDamping() const { return m_Handling.fSuspensionDamping; }
    float GetSuspensionHighSpeedDamping() const { return m_Handling.fSuspensionHighSpdDamping; }
    float GetSuspensionUpperLimit() const { return m_Handling.fSuspensionUpperLimit; }
    float GetSuspensionLowerLimit() const { return m_Handling.fSuspensionLowerLimit; }
    float GetSuspensionFrontRearBias() const { return m_Handling.fSuspensionFrontRearBias; }
    float GetSuspensionAntiDiveMultiplier() const { return m_Handling.fSuspensionAntiDiveMultiplier; }

    float GetCollisionDamageMultiplier() const { return m_Handling.fCollisionDamageMultiplier; }

    unsigned int GetHandlingFlags() const { return m_Handling.uiHandlingFlags; }
    unsigned int GetModelFlags() const { return m_Handling.uiModelFlags; }
    float        GetSeatOffsetDistance() const { return m_Handling.fSeatOffsetDistance; }
    unsigned int GetMonetary() const { return m_Handling.uiMonetary; }

    eLightType    GetHeadLight() const { return static_cast<eLightType>(m_Handling.ucHeadLight); }
    eLightType    GetTailLight() const { return static_cast<eLightType>(m_Handling.ucTailLight); }
    unsigned char GetAnimGroup() const { return m_Handling.ucAnimGroup; }

    std::uint16_t GetVehicleID() const { return static_cast<std::uint16_t>(m_Handling.iVehicleID); }

    // Set functions
    void SetMass(float fMass) { m_Handling.fMass = fMass; }
    void SetTurnMass(float fTurnMass) { m_Handling.fTurnMass = fTurnMass; }
    void SetDragCoeff(float fDrag) { m_Handling.fDragCoeff = fDrag; }
    void SetCenterOfMass(const CVector& vecCenter) { m_Handling.vecCenterOfMass = vecCenter; }

    void SetPercentSubmerged(unsigned int uiPercent) { m_Handling.uiPercentSubmerged = uiPercent; }
    void SetTractionMultiplier(float fTractionMultiplier) { m_Handling.fTractionMultiplier = fTractionMultiplier; }

    void SetCarDriveType(eDriveType Type) { m_Handling.Transmission.ucDriveType = Type; }
    void SetCarEngineType(eEngineType Type) { m_Handling.Transmission.ucEngineType = Type; }
    void SetNumberOfGears(unsigned char ucNumber) { m_Handling.Transmission.ucNumberOfGears = ucNumber; }

    void SetEngineAcceleration(float fAcceleration) { m_Handling.Transmission.fEngineAcceleration = fAcceleration; }
    void SetEngineInertia(float fInertia) { m_Handling.Transmission.fEngineInertia = fInertia; }
    void SetMaxVelocity(float fVelocity) { m_Handling.Transmission.fMaxVelocity = fVelocity; }

    void SetBrakeDeceleration(float fDeceleration) { m_Handling.fBrakeDeceleration = fDeceleration; }
    void SetBrakeBias(float fBias) { m_Handling.fBrakeBias = fBias; }
    void SetABS(bool bABS) { m_Handling.bABS = bABS; }

    void SetSteeringLock(float fSteeringLock) { m_Handling.fSteeringLock = fSteeringLock; }
    void SetTractionLoss(float fTractionLoss) { m_Handling.fTractionLoss = fTractionLoss; }
    void SetTractionBias(float fTractionBias) { m_Handling.fTractionBias = fTractionBias; }

    void SetSuspensionForceLevel(float fForce) noexcept;
    void SetSuspensionDamping(float fDamping) noexcept;
    void SetSuspensionHighSpeedDamping(float fDamping) noexcept;
    void SetSuspensionUpperLimit(float fUpperLimit) noexcept;
    void SetSuspensionLowerLimit(float fLowerLimit) noexcept;
    void SetSuspensionFrontRearBias(float fBias) noexcept;
    void SetSuspensionAntiDiveMultiplier(float fAntidive) noexcept;

    void SetCollisionDamageMultiplier(float fMultiplier) { m_Handling.fCollisionDamageMultiplier = fMultiplier; }

    void SetHandlingFlags(unsigned int uiFlags) { m_Handling.uiHandlingFlags = uiFlags; }
    void SetModelFlags(unsigned int uiFlags) { m_Handling.uiModelFlags = uiFlags; }
    void SetSeatOffsetDistance(float fDistance) { m_Handling.fSeatOffsetDistance = fDistance; }
    void SetMonetary(unsigned int uiMonetary) { m_Handling.uiMonetary = uiMonetary; }

    void SetHeadLight(eLightType Style) { m_Handling.ucHeadLight = Style; }
    void SetTailLight(eLightType Style) { m_Handling.ucTailLight = Style; }
    void SetAnimGroup(unsigned char ucGroup) { m_Handling.ucAnimGroup = ucGroup; }

    void CheckSuspensionChanges() noexcept;

    void Recalculate();

    tHandlingDataSA* GetInterface() const { return m_pHandlingSA; }

private:
    tHandlingDataSA* m_pHandlingSA;
    bool             m_bDeleteInterface;

    tHandlingDataSA m_Handling;
};
