/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/CHandlingEntry.h
 *  PURPOSE:     Vehicle handling entry interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVector;

// ### MODELFLAGS ###
// 1st digit
#define MODELFLAGS_IS_VAN                   0x00000001
#define MODELFLAGS_IS_BUS                   0x00000002
#define MODELFLAGS_IS_LOW                   0x00000004
#define MODELFLAGS_IS_BIG                   0x00000008

// 2nd digit
#define MODELFLAGS_REVERSE_BONNET           0x00000010
#define MODELFLAGS_HANGING_BOOT             0x00000020
#define MODELFLAGS_TAILGATE_BOOT            0x00000040
#define MODELFLAGS_NOSWING_BOOT             0x00000080

// 3rd digit
#define MODELFLAGS_NO_DOORS                 0x00000100
#define MODELFLAGS_TANDEM_SEATS             0x00000200
#define MODELFLAGS_SIT_IN_BOAT              0x00000400
#define MODELFLAGS_CONVERTIBLE              0x00000800

// 4th digit
#define MODELFLAGS_NO_EXHAUST               0x00001000
#define MODELFLAGS_DOUBLE_EXHAUST           0x00002000
#define MODELFLAGS_NO1FPS_LOOK_BEHIND       0x00004000
#define MODELFLAGS_FORCE_DOOR_CHECK         0x00008000

// 5th digit
#define MODELFLAGS_AXLE_F_NOTILT            0x00010000
#define MODELFLAGS_AXLE_F_SOLID             0x00020000
#define MODELFLAGS_AXLE_F_MCPHERSON         0x00040000
#define MODELFLAGS_AXLE_F_REVERSE           0x00080000

// 6th digit
#define MODELFLAGS_AXLE_R_NOTILT            0x00100000
#define MODELFLAGS_AXLE_R_SOLID             0x00200000
#define MODELFLAGS_AXLE_R_MCPHERSON         0x00400000
#define MODELFLAGS_AXLE_R_REVERSE           0x00800000

// 7th digit
#define MODELFLAGS_IS_BIKE                  0x01000000
#define MODELFLAGS_IS_HELI                  0x02000000
#define MODELFLAGS_IS_PLANE                 0x04000000
#define MODELFLAGS_IS_BOAT                  0x08000000

// 8th digit
#define MODELFLAGS_BOUNCE_PANELS            0x10000000
#define MODELFLAGS_DOUBLE_RWHEELS           0x20000000
#define MODELFLAGS_FORCE_GROUND_CLEARANCE   0x40000000
#define MODELFLAGS_IS_HATCHBACK             0x80000000

class CHandlingEntry
{
public:
    enum eDriveType
    {
        FOURWHEEL = '4',
        FWD = 'F',
        RWD = 'R'
    };

    enum eEngineType
    {
        PETROL = 'P',
        DIESEL = 'D',
        ELECTRIC = 'E'
    };

    enum eLightType
    {
        LONG,
        SMALL,
        BIG,
        TALL,
    };

    // Destructor
    virtual ~CHandlingEntry(){};

    // Use this to copy data from an another handling class to this
    virtual void Assign(const CHandlingEntry* pEntry) noexcept = 0;

    // Get functions
    virtual float          GetMass() const noexcept = 0;
    virtual float          GetTurnMass() const noexcept = 0;
    virtual float          GetDragCoeff() const noexcept = 0;
    virtual const CVector& GetCenterOfMass() const noexcept = 0;

    virtual unsigned int GetPercentSubmerged() const noexcept = 0;
    virtual float        GetTractionMultiplier() const noexcept = 0;

    virtual eDriveType    GetCarDriveType() const noexcept = 0;
    virtual eEngineType   GetCarEngineType() const noexcept = 0;
    virtual unsigned char GetNumberOfGears() const noexcept = 0;

    virtual float GetEngineAcceleration() const noexcept = 0;
    virtual float GetEngineInertia() const noexcept = 0;
    virtual float GetMaxVelocity() const noexcept = 0;

    virtual float GetBrakeDeceleration() const noexcept = 0;
    virtual float GetBrakeBias() const noexcept = 0;
    virtual bool  GetABS() const noexcept = 0;

    virtual float GetSteeringLock() const noexcept = 0;
    virtual float GetTractionLoss() const noexcept = 0;
    virtual float GetTractionBias() const noexcept = 0;

    virtual float GetSuspensionForceLevel() const noexcept = 0;
    virtual float GetSuspensionDamping() const noexcept = 0;
    virtual float GetSuspensionHighSpeedDamping() const noexcept = 0;
    virtual float GetSuspensionUpperLimit() const noexcept = 0;
    virtual float GetSuspensionLowerLimit() const noexcept = 0;
    virtual float GetSuspensionFrontRearBias() const noexcept = 0;
    virtual float GetSuspensionAntiDiveMultiplier() const noexcept = 0;

    virtual float GetCollisionDamageMultiplier() const noexcept = 0;

    virtual unsigned int GetHandlingFlags() const noexcept = 0;
    virtual unsigned int GetModelFlags() const noexcept = 0;
    virtual float        GetSeatOffsetDistance() const noexcept = 0;
    virtual unsigned int GetMonetary() const noexcept = 0;

    virtual eLightType    GetHeadLight() const noexcept = 0;
    virtual eLightType    GetTailLight() const noexcept = 0;
    virtual unsigned char GetAnimGroup() const noexcept = 0;

    virtual eHandlingTypes GetVehicleID() const noexcept = 0;

    // Set functions
    virtual void SetMass(float fMass) noexcept = 0;
    virtual void SetTurnMass(float fTurnMass) noexcept = 0;
    virtual void SetDragCoeff(float fDrag) noexcept = 0;
    virtual void SetCenterOfMass(const CVector& vecCenter) noexcept = 0;

    virtual void SetPercentSubmerged(unsigned int uiPercent) noexcept = 0;
    virtual void SetTractionMultiplier(float fTractionMultiplier) noexcept = 0;

    virtual void SetCarDriveType(eDriveType Type) noexcept = 0;
    virtual void SetCarEngineType(eEngineType Type) noexcept = 0;
    virtual void SetNumberOfGears(unsigned char ucNumber) noexcept = 0;

    virtual void SetEngineAcceleration(float fAcceleration) noexcept = 0;
    virtual void SetEngineInertia(float fInertia) noexcept = 0;
    virtual void SetMaxVelocity(float fVelocity) noexcept = 0;

    virtual void SetBrakeDeceleration(float fDeceleration) noexcept = 0;
    virtual void SetBrakeBias(float fBias) noexcept = 0;
    virtual void SetABS(bool bABS) noexcept = 0;

    virtual void SetSteeringLock(float fSteeringLock) noexcept = 0;
    virtual void SetTractionLoss(float fTractionLoss) noexcept = 0;
    virtual void SetTractionBias(float fTractionBias) noexcept = 0;

    virtual void SetSuspensionForceLevel(float fForce) noexcept = 0;
    virtual void SetSuspensionDamping(float fDamping) noexcept = 0;
    virtual void SetSuspensionHighSpeedDamping(float fDamping) noexcept = 0;
    virtual void SetSuspensionUpperLimit(float fUpperLimit) noexcept = 0;
    virtual void SetSuspensionLowerLimit(float fLowerLimit) noexcept = 0;
    virtual void SetSuspensionFrontRearBias(float fBias) noexcept = 0;
    virtual void SetSuspensionAntiDiveMultiplier(float fAntiDive) noexcept = 0;

    virtual void SetCollisionDamageMultiplier(float fMultiplier) noexcept = 0;

    virtual void SetHandlingFlags(unsigned int uiFlags) noexcept = 0;
    virtual void SetModelFlags(unsigned int uiFlags) noexcept = 0;
    virtual void SetSeatOffsetDistance(float fDistance) noexcept = 0;
    virtual void SetMonetary(unsigned int uiMonetary) noexcept = 0;

    virtual void SetHeadLight(eLightType Style) noexcept = 0;
    virtual void SetTailLight(eLightType Style) noexcept = 0;
    virtual void SetAnimGroup(unsigned char ucGroup) noexcept = 0;

    virtual void CheckSuspensionChanges() const noexcept = 0;

    // Call this every time you're done changing something. This will recalculate
    // all transmission/handling values according to the new values.
    virtual void Recalculate() noexcept = 0;
};
