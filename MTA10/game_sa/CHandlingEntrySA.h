/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CHandlingEntrySA.h
*  PURPOSE:		Header file for vehicle handling data entry class
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CHANDLINGENTRY
#define __CGAMESA_CHANDLINGENTRY

#include <game/CHandlingEntry.h>
#include "Common.h"

class CTransmissionSAInterface
{
public:
    float           fUnknown  [18];                 // +40

    unsigned char   ucDriveType         :8;         // +112
    unsigned char   ucEngineType        :8;         // +113
    unsigned char   ucNumberOfGears     :8;         // +114
    unsigned char   ucUnknown           :8;         // +115

    unsigned int    uiHandlingFlags;                // +116

    float           fEngineAccelleration;           // +120     (value in handling.cfg * 0x86A950)
    float           fEngineInertia;                 // +124
    float           fMaxVelocity;                   // +128

    float           fUnknown2 [3];                  // +132
};

struct tHandlingDataSA
{
    int             iUnknown7;                      // +0

    float           fMass;                          // +4

    float           fUnknown1;                      // +8    Automatically calculated

    float           fTurnMass;                      // +12
    float           fDragCoeff;                     // +16
    CVector         vecCenterOfMass;                // +20
    unsigned int    uiPercentSubmerged;             // +32

    float           fUnknown2;                      // +36  Automatically calculated

    float           fTractionMultiplier;            // +40

    CTransmissionSAInterface Transmission;          // +44
    float           fBrakeDecelleration;            // +148
    float           fBrakeBias;                     // +152
    bool            bABS;                           // +156

    float           fSteeringLock;                  // +160
    float           fTractionLoss;                  // +164
    float           fTractionBias;                  // +168

    float           fSuspensionForceLevel;          // +172
    float           fSuspensionDamping;             // +176
    float           fSuspensionHighSpdDamping;      // +180
    float           fSuspensionUpperLimit;          // +184
    float           fSuspensionLowerLimit;          // +188
    float           fSuspensionFrontRearBias;       // +192
    float           fSuspensionAntidiveMultiplier;  // +196

    float           fCollisionDamageMultiplier;     // +200

    unsigned int    uiModelFlags;                   // +204
    unsigned int    uiHandlingFlags;                // +208
    float           fSeatOffsetDistance;            // +212
    unsigned int    uiMonetary;                     // +216

    unsigned char   ucHeadLight     : 8;            // +220
    unsigned char   ucTailLight     : 8;            // +221
    unsigned char   ucAnimGroup     : 8;            // +222
    unsigned char   ucUnused        : 8;            // +223
};


class CHandlingEntrySA : public CHandlingEntry
{
public:
                    // Constructor for creatable dummy entries
                    CHandlingEntrySA                ( void );

                    // Constructor for game linked entries
                    CHandlingEntrySA                ( tHandlingDataSA* pDataSA, tHandlingDataSA* pOriginalUncalculatedData );

                    // Constructor for original entries
                    CHandlingEntrySA                ( tHandlingDataSA* pOriginal );

    virtual         ~CHandlingEntrySA               ( void );

    // Use this to copy data from an another handling class to this
    void            ApplyHandlingData               ( CHandlingEntry* pData );

    // Get functions
    float           GetMass                         ( void ) const    { return m_Handling.fMass; };
    float           GetTurnMass                     ( void ) const    { return m_Handling.fTurnMass; };
    float           GetDragCoeff                    ( void ) const    { return m_Handling.fDragCoeff; };
    const CVector&  GetCenterOfMass                 ( void ) const    { return m_Handling.vecCenterOfMass; };

    unsigned int    GetPercentSubmerged             ( void ) const    { return m_Handling.uiPercentSubmerged; };
    float           GetTractionMultiplier           ( void ) const    { return m_Handling.fTractionMultiplier; };

    eDriveType      GetCarDriveType                 ( void ) const    { return static_cast < eDriveType > ( m_Handling.Transmission.ucDriveType ); };
    eEngineType     GetCarEngineType                ( void ) const    { return static_cast < eEngineType > ( m_Handling.Transmission.ucEngineType ); };
    unsigned char   GetNUmberOfGears                ( void ) const    { return m_Handling.Transmission.ucNumberOfGears; };

    float           GetEngineAccelleration          ( void ) const    { return m_Handling.Transmission.fEngineAccelleration; };
    float           GetEngineInertia                ( void ) const    { return m_Handling.Transmission.fEngineInertia; };
    float           GetMaxVelocity                  ( void ) const    { return m_Handling.Transmission.fMaxVelocity; };

    float           GetBrakeDecelleration           ( void ) const    { return m_Handling.fBrakeDecelleration; };
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
    float           GetSuspensionAntidiveMultiplier ( void ) const    { return m_Handling.fSuspensionAntidiveMultiplier; };

    float           GetCollisionDamageMultiplier    ( void ) const    { return m_Handling.fCollisionDamageMultiplier; };

    unsigned int    GetHandlingFlags                ( void ) const    { return m_Handling.uiHandlingFlags; };
    unsigned int    GetModelFlags                   ( void ) const    { return m_Handling.uiModelFlags; };
    float           GetSeatOffsetDistance           ( void ) const    { return m_Handling.fSeatOffsetDistance; };
    unsigned int    GetMonetary                     ( void ) const    { return m_Handling.uiMonetary; };

    eLightType      GetHeadLight                    ( void ) const    { return static_cast < eLightType > ( m_Handling.ucHeadLight ); };
    eLightType      GetTailLight                    ( void ) const    { return static_cast < eLightType > ( m_Handling.ucTailLight ); };
    unsigned char   GetAnimGroup                    ( void ) const    { return m_Handling.ucAnimGroup; };


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

    void            SetEngineAccelleration          ( float fAccelleration )        { m_Handling.Transmission.fEngineAccelleration = fAccelleration; };
    void            SetEngineInertia                ( float fInertia )              { m_Handling.Transmission.fEngineInertia = fInertia; };
    void            SetMaxVelocity                  ( float fVelocity )             { m_Handling.Transmission.fMaxVelocity = fVelocity; };
    
    void            SetBrakeDecelleration           ( float fDecelleration )        { m_Handling.fBrakeDecelleration = fDecelleration; };
    void            SetBrakeBias                    ( float fBias )                 { m_Handling.fBrakeBias = fBias; };
    void            SetABS                          ( bool bABS )                   { m_Handling.bABS = bABS; };

    void            SetSteeringLock                 ( float fSteeringLock )         { m_Handling.fSteeringLock = fSteeringLock; };
    void            SetTractionLoss                 ( float fTractionLoss )         { m_Handling.fTractionLoss = fTractionLoss; };
    void            SetTractionBias                 ( float fTractionBias )         { m_Handling.fTractionBias = fTractionBias; };

    void            SetSuspensionForceLevel         ( float fForce )                { m_Handling.fSuspensionForceLevel = fForce; };
    void            SetSuspensionDamping            ( float fDamping )              { m_Handling.fSuspensionDamping = fDamping; };
    void            SetSuspensionHighSpeedDamping   ( float fDamping )              { m_Handling.fSuspensionHighSpdDamping = fDamping; };
    void            SetSuspensionUpperLimit         ( float fUpperLimit )           { m_Handling.fSuspensionUpperLimit = fUpperLimit; };
    void            SetSuspensionLowerLimit         ( float fLowerLimit )           { m_Handling.fSuspensionLowerLimit = fLowerLimit; };
    void            SetSuspensionFrontRearBias      ( float fBias )                 { m_Handling.fSuspensionFrontRearBias = fBias; };
    void            SetSuspensionAntidiveMultiplier ( float fAntidive )             { m_Handling.fSuspensionAntidiveMultiplier = fAntidive; };

    void            SetCollisionDamageMultiplier    ( float fMultiplier )           { m_Handling.fCollisionDamageMultiplier = fMultiplier; };

    void            SetHandlingFlags                ( unsigned int uiFlags )        { m_Handling.uiHandlingFlags = uiFlags; };
    void            SetModelFlags                   ( unsigned int uiFlags )        { m_Handling.uiModelFlags = uiFlags; };
    void            SetSeatOffsetDistance           ( float fDistance )             { m_Handling.fSeatOffsetDistance = fDistance; };
    void            SetMonetary                     ( unsigned int uiMonetary )     { m_Handling.uiMonetary = uiMonetary; };

    void            SetHeadLight                    ( eLightType Style )            { m_Handling.ucHeadLight = Style; };
    void            SetTailLight                    ( eLightType Style )            { m_Handling.ucTailLight = Style; };
    void            SetAnimGroup                    ( unsigned char ucGroup )       { m_Handling.ucAnimGroup = ucGroup; };

    void            Recalculate                     ( void );

    void            Restore                         ( void );

    tHandlingDataSA*    GetInterface                ( void )                        { return m_pHandlingSA; };

private:
    tHandlingDataSA*        m_pHandlingSA;
    bool                    m_bDeleteInterface;

    tHandlingDataSA         m_Handling;

    tHandlingDataSA*        m_pOriginalData;
};

#endif