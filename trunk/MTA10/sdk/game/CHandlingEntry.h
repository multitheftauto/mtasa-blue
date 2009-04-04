/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CHandlingEntry.h
*  PURPOSE:		Vehicle handling entry interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHANDLINGENTRY
#define __CHANDLINGENTRY

#include <CVector.h>

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
    virtual                 ~CHandlingEntry                 ( void ) {};

    // Use this to copy data from an another handling class to this
    virtual void            ApplyHandlingData               ( CHandlingEntry* pData ) = 0;

    // Get functions
    virtual float           GetMass                         ( void ) const = 0;
    virtual float           GetTurnMass                     ( void ) const = 0;
    virtual float           GetDragCoeff                    ( void ) const = 0;
    virtual const CVector&  GetCenterOfMass                 ( void ) const = 0;

    virtual unsigned int    GetPercentSubmerged             ( void ) const = 0;
    virtual float           GetTractionMultiplier           ( void ) const = 0;

    virtual eDriveType      GetCarDriveType                 ( void ) const = 0;
    virtual eEngineType     GetCarEngineType                ( void ) const = 0;
    virtual unsigned char   GetNUmberOfGears                ( void ) const = 0;

    virtual float           GetEngineAccelleration          ( void ) const = 0;
    virtual float           GetEngineInertia                ( void ) const = 0;
    virtual float           GetMaxVelocity                  ( void ) const = 0;

    virtual float           GetBrakeDecelleration           ( void ) const = 0;
    virtual float           GetBrakeBias                    ( void ) const = 0;
    virtual bool            GetABS                          ( void ) const = 0;

    virtual float           GetSteeringLock                 ( void ) const = 0;
    virtual float           GetTractionLoss                 ( void ) const = 0;
    virtual float           GetTractionBias                 ( void ) const = 0;

    virtual float           GetSuspensionForceLevel         ( void ) const = 0;
    virtual float           GetSuspensionDamping            ( void ) const = 0;
    virtual float           GetSuspensionHighSpeedDamping   ( void ) const = 0;
    virtual float           GetSuspensionUpperLimit         ( void ) const = 0;
    virtual float           GetSuspensionLowerLimit         ( void ) const = 0;
    virtual float           GetSuspensionFrontRearBias      ( void ) const = 0;
    virtual float           GetSuspensionAntidiveMultiplier ( void ) const = 0;

    virtual float           GetCollisionDamageMultiplier    ( void ) const = 0;

    virtual unsigned int    GetHandlingFlags                ( void ) const = 0;
    virtual unsigned int    GetModelFlags                   ( void ) const = 0;
    virtual float           GetSeatOffsetDistance           ( void ) const = 0;
    virtual unsigned int    GetMonetary                     ( void ) const = 0;

    virtual eLightType      GetHeadLight                    ( void ) const = 0;
    virtual eLightType      GetTailLight                    ( void ) const = 0;
    virtual unsigned char   GetAnimGroup                    ( void ) const = 0;


    // Set functions
    virtual void            SetMass                         ( float fMass ) = 0;
    virtual void            SetTurnMass                     ( float fTurnMass ) = 0;
    virtual void            SetDragCoeff                    ( float fDrag ) = 0;
    virtual void            SetCenterOfMass                 ( const CVector& vecCenter ) = 0;

    virtual void            SetPercentSubmerged             ( unsigned int uiPercent ) = 0;
    virtual void            SetTractionMultiplier           ( float fTractionMultiplier ) = 0;

    virtual void            SetCarDriveType                 ( eDriveType Type ) = 0;
    virtual void            SetCarEngineType                ( eEngineType Type ) = 0;
    virtual void            SetNumberOfGears                ( unsigned char ucNumber ) = 0;

    virtual void            SetEngineAccelleration          ( float fAccelleration ) = 0;
    virtual void            SetEngineInertia                ( float fInertia ) = 0;
    virtual void            SetMaxVelocity                  ( float fVelocity ) = 0;
    
    virtual void            SetBrakeDecelleration           ( float fDecelleration ) = 0;
    virtual void            SetBrakeBias                    ( float fBias ) = 0;
    virtual void            SetABS                          ( bool bABS ) = 0;

    virtual void            SetSteeringLock                 ( float fSteeringLock ) = 0;
    virtual void            SetTractionLoss                 ( float fTractionLoss ) = 0;
    virtual void            SetTractionBias                 ( float fTractionBias ) = 0;

    virtual void            SetSuspensionForceLevel         ( float fForce ) = 0;
    virtual void            SetSuspensionDamping            ( float fDamping ) = 0;
    virtual void            SetSuspensionHighSpeedDamping   ( float fDamping ) = 0;
    virtual void            SetSuspensionUpperLimit         ( float fUpperLimit ) = 0;
    virtual void            SetSuspensionLowerLimit         ( float fLowerLimit ) = 0;
    virtual void            SetSuspensionFrontRearBias      ( float fBias ) = 0;
    virtual void            SetSuspensionAntidiveMultiplier ( float fAntidive ) = 0;

    virtual void            SetCollisionDamageMultiplier    ( float fMultiplier ) = 0;

    virtual void            SetHandlingFlags                ( unsigned int uiFlags ) = 0;
    virtual void            SetModelFlags                   ( unsigned int uiFlags ) = 0;
    virtual void            SetSeatOffsetDistance           ( float fDistance ) = 0;
    virtual void            SetMonetary                     ( unsigned int uiMonetary ) = 0;

    virtual void            SetHeadLight                    ( eLightType Style ) = 0;
    virtual void            SetTailLight                    ( eLightType Style ) = 0;
    virtual void            SetAnimGroup                    ( unsigned char ucGroup ) = 0;


    // Call this every time you're done changing something. This will recalculate
    // all transmission/handling values according to the new values.
    virtual void            Recalculate                     ( void ) = 0;

    // This will restore this handling data back to default values.
    virtual void            Restore                         ( void ) = 0;
};

#endif
