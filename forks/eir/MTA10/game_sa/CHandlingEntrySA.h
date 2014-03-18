/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHandlingEntrySA.h
*  PURPOSE:     Header file for vehicle handling data entry class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CHANDLINGENTRY
#define __CGAMESA_CHANDLINGENTRY

#include <game/CHandlingEntry.h>
#include "Common.h"
#define FUNC_HandlingDataMgr_ConvertBikeDataToGameUnits 0x6F5290
#define FUNC_HandlingDataMgr_ConvertDataToGameUnits 0x6F5080
// http://www.gtamodding.com/index.php?title=Handling.cfg#GTA_San_Andreas
// http://www.gtamodding.com/index.php?title=Memory_Addresses_%28SA%29#Handling

class CTransmissionSAInterface
{
public:
    float           fUnknown  [18];                 // +40

    unsigned char   ucDriveType         :8;         // +112
    unsigned char   ucEngineType        :8;         // +113
    unsigned char   ucNumberOfGears     :8;         // +114
    unsigned char   ucUnknown           :8;         // +115

    unsigned int    uiHandlingFlags;                // +116

    float           fEngineAcceleration;            // +120     (value in handling.cfg * 0x86A950)
    float           fEngineInertia;                 // +124
    float           fMaxVelocity;                   // +128

    float           fUnknown2 [3];                  // +132
};

#define MODEL_ISVAN                 0x00000001
#define MODEL_ISBUS                 0x00000002
#define MODEL_ISLOW                 0x00000004
#define MODEL_ISBIG                 0x00000008
#define MODEL_REVERSEBONNET         0x00000010
#define MODEL_BOOT_TOP              0x00000020
#define MODEL_BOOT_BOTTOM           0x00000040
#define MODEL_BOOT_LOCKED           0x00000080
#define MODEL_NODOORS               0x00000100
#define MODEL_TANDEMSEATS           0x00000200
#define MODEL_BOAT_SIT              0x00000400
#define MODEL_SPECIAL               0x00000800
#define MODEL_NOEXHAUST             0x00001000
#define MODEL_DOUBLE_EXHAUST        0x00002000
#define MODEL_NOREARWINDOW          0x00004000
#define MODEL_DOORCHECK             0x00008000
#define MODEL_SUSP_FRONT_STATIC     0x00010000
#define MODEL_SUSP_FRONT_PARALLEL   0x00020000
#define MODEL_SUSP_FRONT_TILT       0x00040000
#define MODEL_SUSP_FRONT_REVERSE    0x00080000
#define MODEL_SUSP_REAR_STATIC      0x00100000
#define MODEL_SUSP_REAR_PARALLEL    0x00200000
#define MODEL_SUSP_REAR_TILT        0x00400000
#define MODEL_SUSP_REAR_REVERSE     0x00800000
#define MODEL_ISBIKE                0x01000000
#define MODEL_ISHELI                0x02000000
#define MODEL_ISPLANE               0x04000000
#define MODEL_ISBOAT                0x08000000
#define MODEL_BOUNCYPANELS          0x10000000
#define MODEL_REINFORCED_REAR       0x20000000
#define MODEL_GROUNDCHECK           0x40000000
#define MODEL_ISHATCHBACK           0x80000000

#define HANDLING_BOOST              0x00000001
#define HANDLING_BOOST_2            0x00000002
#define HANDLING_ANTIROLL           0x00000004
#define HANDLING_NORMALIZE          0x00000008
#define HANDLING_NOHANDBRAKE        0x00000010
#define HANDLING_STEERREAR          0x00000020
#define HANDLING_HBSTEERREAR        0x00000040
#define HANDLING_STEER_OPTIMIZE     0x00000080
#define HANDLING_FRONTWHEEL_THIN    0x00000100
#define HANDLING_FRONTWHEEL_NARROW  0x00000200
#define HANDLING_FRONTWHEEL_WIDE    0x00000400
#define HANDLING_FRONTWHEEL_BROAD   0x00000800
#define HANDLING_REARWHEEL_THIN     0x00001000
#define HANDLING_REARWHEEL_NARROW   0x00002000
#define HANDLING_REARWHEEL_WIDE     0x00004000
#define HANDLING_REARWHEEL_BROAD    0x00008000
#define HANDLING_HYDRAULICS_GEOM    0x00010000
#define HANDLING_HYDRAULICS         0x00020000
#define HANDLING_ANTIHYDRAULICS     0x00040000
#define HANDLING_NOS                0x00080000
#define HANDLING_DIRTRIDER          0x00100000
#define HANDLING_SANDRIDER          0x00200000
#define HANDLING_HALOGENLIGHTS      0x00400000
#define HANDLING_PROC_REARWHEEL     0x00800000
#define HANDLING_LIMIT_VELOCITY     0x01000000
#define HANDLING_LOWRIDER           0x02000000
#define HANDLING_STREETRACER        0x04000000
#define HANDLING_UNKNOWN            0x08000000
#define HANDLING_SWINGINGCHASIS     0x10000000
//todo: 4 unk flags

struct tHandlingDataSA
{
    int             iVehicleID;                     // +0

    float           fMass;                          // +4

    float           fUnknown1;                      // +8    Automatically calculated

    float           fTurnMass;                      // +12
    float           fDragCoeff;                     // +16
    CVector         vecCenterOfMass;                // +20
    unsigned int    uiPercentSubmerged;             // +32

    float           fUnknown2;                      // +36  Automatically calculated

    float           fTractionMultiplier;            // +40

    CTransmissionSAInterface Transmission;          // +44
    float           fBrakeDeceleration;             // +148
    float           fBrakeBias;                     // +152
    bool            bABS;                           // +156
    char            fUnknown[3];                    // +157

    float           fSteeringLock;                  // +160
    float           fTractionLoss;                  // +164
    float           fTractionBias;                  // +168

    float           fSuspensionForceLevel;          // +172
    float           fSuspensionDamping;             // +176
    float           fSuspensionHighSpdDamping;      // +180
    float           fSuspensionUpperLimit;          // +184
    float           fSuspensionLowerLimit;          // +188
    float           fSuspensionFrontRearBias;       // +192
    float           fSuspensionAntiDiveMultiplier;  // +196

    float           fCollisionDamageMultiplier;     // +200

    unsigned int    uiModelFlags;                   // +204
    unsigned int    uiHandlingFlags;                // +208
    float           fSeatOffsetDistance;            // +212
    unsigned int    uiMonetary;                     // +216

    unsigned char   ucHeadLight     : 8;            // +220
    unsigned char   ucTailLight     : 8;            // +221
    unsigned char   ucAnimGroup     : 8;            // +222
};


class CHandlingEntrySA : public CHandlingEntry
{
public:
                    // Constructor for creatable dummy entries
                    CHandlingEntrySA                ( void );

                    // Constructor for original entries
                    CHandlingEntrySA                ( tHandlingDataSA* pOriginal );

    virtual         ~CHandlingEntrySA               ( void );

    // Use this to copy data from an another handling class to this
    void            Assign                          ( const CHandlingEntry* pData );

    // Get functions
    float           GetMass                         ( void ) const    { return m_Handling.fMass; };
    float           GetTurnMass                     ( void ) const    { return m_Handling.fTurnMass; };
    float           GetDragCoeff                    ( void ) const    { return m_Handling.fDragCoeff; };
    const CVector&  GetCenterOfMass                 ( void ) const    { return m_Handling.vecCenterOfMass; };

    unsigned int    GetPercentSubmerged             ( void ) const    { return m_Handling.uiPercentSubmerged; };
    float           GetTractionMultiplier           ( void ) const    { return m_Handling.fTractionMultiplier; };

    eDriveType      GetCarDriveType                 ( void ) const    { return static_cast < eDriveType > ( m_Handling.Transmission.ucDriveType ); };
    eEngineType     GetCarEngineType                ( void ) const    { return static_cast < eEngineType > ( m_Handling.Transmission.ucEngineType ); };
    unsigned char   GetNumberOfGears                ( void ) const    { return m_Handling.Transmission.ucNumberOfGears; };

    float           GetEngineAcceleration           ( void ) const    { return m_Handling.Transmission.fEngineAcceleration; };
    float           GetEngineInertia                ( void ) const    { return m_Handling.Transmission.fEngineInertia; };
    float           GetMaxVelocity                  ( void ) const    { return m_Handling.Transmission.fMaxVelocity; };

    float           GetBrakeDeceleration            ( void ) const    { return m_Handling.fBrakeDeceleration; };
    float           GetBrakeBias                    ( void ) const    { return m_Handling.fBrakeBias; };
    bool            GetABS                          ( void ) const    { return m_Handling.bABS; };

    float           GetSteeringLock                 ( void ) const    { return m_Handling.fSteeringLock; };
    float           GetTractionLoss                 ( void ) const    { return m_Handling.fTractionLoss; };
    float           GetTractionBias                 ( void ) const    { return m_Handling.fTractionBias; };

    float           GetSuspensionForceLevel         ( void ) const    { return m_Handling.fSuspensionForceLevel; };
    float           GetSuspensionDamping            ( void ) const    { return m_Handling.fSuspensionDamping; };
    float           GetSuspensionHighSpeedDamping   ( void ) const    { return m_Handling.fSuspensionHighSpdDamping; };
    float           GetSuspensionUpperLimit         ( void ) const    { return m_Handling.fSuspensionUpperLimit; };
    float           GetSuspensionLowerLimit         ( void ) const    { return m_Handling.fSuspensionLowerLimit; };
    float           GetSuspensionFrontRearBias      ( void ) const    { return m_Handling.fSuspensionFrontRearBias; };
    float           GetSuspensionAntiDiveMultiplier ( void ) const    { return m_Handling.fSuspensionAntiDiveMultiplier; };

    float           GetCollisionDamageMultiplier    ( void ) const    { return m_Handling.fCollisionDamageMultiplier; };

    unsigned int    GetHandlingFlags                ( void ) const    { return m_Handling.uiHandlingFlags; };
    unsigned int    GetModelFlags                   ( void ) const    { return m_Handling.uiModelFlags; };
    float           GetSeatOffsetDistance           ( void ) const    { return m_Handling.fSeatOffsetDistance; };
    unsigned int    GetMonetary                     ( void ) const    { return m_Handling.uiMonetary; };

    eLightType      GetHeadLight                    ( void ) const    { return static_cast < eLightType > ( m_Handling.ucHeadLight ); };
    eLightType      GetTailLight                    ( void ) const    { return static_cast < eLightType > ( m_Handling.ucTailLight ); };
    unsigned char   GetAnimGroup                    ( void ) const    { return m_Handling.ucAnimGroup; };

    eVehicleTypes   GetModel                        ( void ) const    { return static_cast < eVehicleTypes > ( m_Handling.iVehicleID ); };
    bool            HasSuspensionChanged            ( void ) const    { return true; };

    // Set functions
    void            SetMass                         ( float fMass )                 { m_Handling.fMass = fMass; };
    void            SetTurnMass                     ( float fTurnMass )             { m_Handling.fTurnMass = fTurnMass; };
    void            SetDragCoeff                    ( float fDrag )                 { m_Handling.fDragCoeff = fDrag; };
    void            SetCenterOfMass                 ( const CVector& vecCenter )    { m_Handling.vecCenterOfMass = vecCenter; };

    void            SetPercentSubmerged             ( unsigned int uiPercent )      { m_Handling.uiPercentSubmerged = uiPercent; };
    void            SetTractionMultiplier           ( float fTractionMultiplier )   { m_Handling.fTractionMultiplier = fTractionMultiplier; };

    void            SetCarDriveType                 ( eDriveType Type )             { m_Handling.Transmission.ucDriveType = Type; };
    void            SetCarEngineType                ( eEngineType Type )            { m_Handling.Transmission.ucEngineType = Type; };
    void            SetNumberOfGears                ( unsigned char ucNumber )      { m_Handling.Transmission.ucNumberOfGears = ucNumber; };

    void            SetEngineAcceleration           ( float fAcceleration )         { m_Handling.Transmission.fEngineAcceleration = fAcceleration; };
    void            SetEngineInertia                ( float fInertia )              { m_Handling.Transmission.fEngineInertia = fInertia; };
    void            SetMaxVelocity                  ( float fVelocity )             { m_Handling.Transmission.fMaxVelocity = fVelocity; };
    
    void            SetBrakeDeceleration            ( float fDeceleration )         { m_Handling.fBrakeDeceleration = fDeceleration; };
    void            SetBrakeBias                    ( float fBias )                 { m_Handling.fBrakeBias = fBias; };
    void            SetABS                          ( bool bABS )                   { m_Handling.bABS = bABS; };

    void            SetSteeringLock                 ( float fSteeringLock )         { m_Handling.fSteeringLock = fSteeringLock; };
    void            SetTractionLoss                 ( float fTractionLoss )         { m_Handling.fTractionLoss = fTractionLoss; };
    void            SetTractionBias                 ( float fTractionBias )         { m_Handling.fTractionBias = fTractionBias; };

    void            SetSuspensionForceLevel         ( float fForce );
    void            SetSuspensionDamping            ( float fDamping );
    void            SetSuspensionHighSpeedDamping   ( float fDamping );
    void            SetSuspensionUpperLimit         ( float fUpperLimit );
    void            SetSuspensionLowerLimit         ( float fLowerLimit );
    void            SetSuspensionFrontRearBias      ( float fBias );
    void            SetSuspensionAntiDiveMultiplier ( float fAntidive );

    void            SetCollisionDamageMultiplier    ( float fMultiplier )           { m_Handling.fCollisionDamageMultiplier = fMultiplier; };

    void            SetHandlingFlags                ( unsigned int uiFlags )        { m_Handling.uiHandlingFlags = uiFlags; };
    void            SetModelFlags                   ( unsigned int uiFlags )        { m_Handling.uiModelFlags = uiFlags; };
    void            SetSeatOffsetDistance           ( float fDistance )             { m_Handling.fSeatOffsetDistance = fDistance; };
    void            SetMonetary                     ( unsigned int uiMonetary )     { m_Handling.uiMonetary = uiMonetary; };

    void            SetHeadLight                    ( eLightType Style )            { m_Handling.ucHeadLight = Style; };
    void            SetTailLight                    ( eLightType Style )            { m_Handling.ucTailLight = Style; };
    void            SetAnimGroup                    ( unsigned char ucGroup )       { m_Handling.ucAnimGroup = ucGroup; };

    void            Recalculate                     ( unsigned short usModel );

    tHandlingDataSA*    GetInterface                ( void )                        { return m_pHandlingSA; };

    void            SetSuspensionChanged            ( bool bChanged )               { m_bChanged = bChanged; };

private:
    tHandlingDataSA*        m_pHandlingSA;
    bool                    m_bDeleteInterface;

    tHandlingDataSA         m_Handling;

    tHandlingDataSA*        m_pOriginalData;
    bool                    m_bChanged;
};

// sizeof(tFlyingHandlingDataSA) == 0x58
struct tFlyingHandlingDataSA
{
    int         iVehicleID;
    float       fThrust;
    float       fThrustFallOff;
    float       fYaw;
    float       fStab;
    float       fSideSlip;
    float       fRoll;
    float       fRollStab;
    float       fPitch;
    float       fPitchStab;
    float       fFormLift;
    float       fAttackLift;
    float       GearUpR;
    float       GearDownL;
    float       WindMult;
    float       MoveResistance;
    CVector     TurnResistance;
    CVector     SpeedResistance;
};

#endif
