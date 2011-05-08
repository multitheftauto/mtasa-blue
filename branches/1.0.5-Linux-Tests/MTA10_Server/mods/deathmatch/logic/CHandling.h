/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHandling.h
*  PURPOSE:     Vehicle handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHANDLING_H
#define __CHANDLING_H

#include "CElement.h"
#include "CEvents.h"
#include "Utils.h"

class CHandling : public CElement
{
    friend class CHandlingManager;

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

                                CHandling                       ( CElement* pParent, CXMLNode* pNode, class CHandlingManager* pHandlingManager );
                                CHandling                       ( const CHandling& Copy );
                                ~CHandling                      ( void );

    bool                        IsEntity                        ( void )                    { return true; }

    void                        Unlink                          ( void );
    bool                        ReadSpecialData                 ( void );

    // Vehicles this handling is defaulted to
    size_t                      CountDefaultTo                  ( void )    { return m_DefaultTo.size (); };
    void                        AddDefaultTo                    ( unsigned short usID );
    void                        RemoveDefaultTo                 ( unsigned short usID )     { m_DefaultTo.remove ( usID ); };
    bool                        IsDefaultFor                    ( unsigned short usID );
    std::list < unsigned short >::const_iterator    IterDefaultToBegin  ( void )            { return m_DefaultTo.begin (); };
    std::list < unsigned short >::const_iterator    IterDefaultToEnd    ( void )            { return m_DefaultTo.end (); };

    // Get functions
    float                       GetMass                         ( bool& bChanged );
    float                       GetTurnMass                     ( bool& bChanged );
    float                       GetDragCoeff                    ( bool& bChanged );
    const CVector&              GetCenterOfMass                 ( bool& bChanged );

    unsigned int                GetPercentSubmerged             ( bool& bChanged );
    float                       GetTractionMultiplier           ( bool& bChanged );

    eDriveType                  GetDriveType                    ( bool& bChanged );
    eEngineType                 GetEngineType                   ( bool& bChanged );
    unsigned char               GetNumberOfGears                ( bool& bChanged );

    float                       GetEngineAccelleration          ( bool& bChanged );
    float                       GetEngineInertia                ( bool& bChanged );
    float                       GetMaxVelocity                  ( bool& bChanged );

    float                       GetBrakeDecelleration           ( bool& bChanged );
    float                       GetBrakeBias                    ( bool& bChanged );
    bool                        GetABS                          ( bool& bChanged );

    float                       GetSteeringLock                 ( bool& bChanged );
    float                       GetTractionLoss                 ( bool& bChanged );
    float                       GetTractionBias                 ( bool& bChanged );

    float                       GetSuspensionForceLevel         ( bool& bChanged );
    float                       GetSuspensionDamping            ( bool& bChanged );
    float                       GetSuspensionHighSpeedDamping   ( bool& bChanged );
    float                       GetSuspensionUpperLimit         ( bool& bChanged );
    float                       GetSuspensionLowerLimit         ( bool& bChanged );
    float                       GetSuspensionFrontRearBias      ( bool& bChanged );
    float                       GetSuspensionAntidiveMultiplier ( bool& bChanged );

    float                       GetCollisionDamageMultiplier    ( bool& bChanged );

    unsigned int                GetHandlingFlags                ( bool& bChanged );
    unsigned int                GetModelFlags                   ( bool& bChanged );
    float                       GetSeatOffsetDistance           ( bool& bChanged );

    eLightType                  GetHeadLight                    ( bool& bChanged );
    eLightType                  GetTailLight                    ( bool& bChanged );
    unsigned char               GetAnimGroup                    ( bool& bChanged );


    // Set functions
    void                        SetMass                         ( float fMass )                 { m_fMass = fMass; m_bMassChanged = true; };
    void                        SetTurnMass                     ( float fTurnMass )             { m_fTurnMass = fTurnMass; m_bTurnMassChanged = true; };
    void                        SetDragCoeff                    ( float fDrag )                 { m_fDragCoeff = fDrag; m_bDragCoeffChanged = true; };
    void                        SetCenterOfMass                 ( const CVector& vecCenter )    { m_vecCenterOfMass = vecCenter; m_bCenterOfMassChanged = true; };

    void                        SetPercentSubmerged             ( unsigned int uiPercent )      { m_uiPercentSubmerged = uiPercent; m_bPercentSubmergedChanged = true; };
    void                        SetTractionMultiplier           ( float fTractionMultiplier )   { m_fTractionMultiplier = fTractionMultiplier; m_bTractionMultiplierChanged = true; };

    void                        SetDriveType                    ( eDriveType Type )             { m_ucDriveType = Type; m_bDriveTypeChanged = true; };
    void                        SetEngineType                   ( eEngineType Type )            { m_ucEngineType = Type; m_bEngineTypeChanged = true; };
    void                        SetNumberOfGears                ( unsigned char ucNumber )      { m_ucNumberOfGears = ucNumber; m_bNumberOfGearsChanged = true; };

    void                        SetEngineAccelleration          ( float fAccelleration )        { m_fEngineAccelleration = fAccelleration; m_bEngineAccellerationChanged = true; };
    void                        SetEngineInertia                ( float fInertia )              { m_fEngineInertia = fInertia; m_bEngineInertiaChanged = true; };
    void                        SetMaxVelocity                  ( float fVelocity )             { m_fMaxVelocity = fVelocity; m_bMaxVelocityChanged = true; };
    
    void                        SetBrakeDecelleration           ( float fDecelleration )        { m_fBrakeDecelleration = fDecelleration; m_bBrakeDecellerationChanged = true; };
    void                        SetBrakeBias                    ( float fBias )                 { m_fBrakeBias = fBias; m_bBrakeBiasChanged = true; };
    void                        SetABS                          ( bool bABS )                   { m_bABS = bABS; m_bABSChanged = true; };

    void                        SetSteeringLock                 ( float fSteeringLock )         { m_fSteeringLock = fSteeringLock; m_bSteeringLockChanged = true; };
    void                        SetTractionLoss                 ( float fTractionLoss )         { m_fTractionLoss = fTractionLoss; m_bTractionLossChanged = true; };
    void                        SetTractionBias                 ( float fTractionBias )         { m_fTractionBias = fTractionBias; m_bTractionBiasChanged = true; };

    void                        SetSuspensionForceLevel         ( float fForce )                { m_fSuspensionForceLevel = fForce; m_bSuspensionForceLevelChanged = true; };
    void                        SetSuspensionDamping            ( float fDamping )              { m_fSuspensionDamping = fDamping; m_bSuspensionDampingChanged = true; };
    void                        SetSuspensionHighSpeedDamping   ( float fDamping )              { m_fSuspensionHighSpdDamping = fDamping; m_bSuspensionHighSpdDampingChanged = true; };
    void                        SetSuspensionUpperLimit         ( float fUpperLimit )           { m_fSuspensionUpperLimit = fUpperLimit; m_bSuspensionUpperLimitChanged = true; };
    void                        SetSuspensionLowerLimit         ( float fLowerLimit )           { m_fSuspensionLowerLimit = fLowerLimit; m_bSuspensionLowerLimitChanged = true; };
    void                        SetSuspensionFrontRearBias      ( float fBias )                 { m_fSuspensionFrontRearBias = fBias; m_bSuspensionFrontRearBiasChanged = true; };
    void                        SetSuspensionAntidiveMultiplier ( float fAntidive )             { m_fSuspensionAntidiveMultiplier = fAntidive; m_bSuspensionAntidiveMultiplierChanged = true; };

    void                        SetCollisionDamageMultiplier    ( float fMultiplier )           { m_fCollisionDamageMultiplier = fMultiplier; m_bCollisionDamageMultiplierChanged = true; };

    void                        SetHandlingFlags                ( unsigned int uiFlags )        { m_uiHandlingFlags = uiFlags; m_bHandlingFlagsChanged = true; };
    void                        SetModelFlags                   ( unsigned int uiFlags )        { m_uiModelFlags = uiFlags; m_bModelFlagsChanged = true; };
    void                        SetSeatOffsetDistance           ( float fDistance )             { m_fSeatOffsetDistance = fDistance; m_bSeatOffsetDistanceChanged = true; };

    void                        SetHeadLight                    ( eLightType Style )            { m_ucHeadLight = Style; m_bHeadLightChanged = true; };
    void                        SetTailLight                    ( eLightType Style )            { m_ucTailLight = Style; m_bTailLightChanged = true; };
    void                        SetAnimGroup                    ( unsigned char ucGroup )       { m_ucAnimGroup = ucGroup; m_bAnimGroupChanged = true; };

    // Restore funcs
    void                        RestoreMass                         ( void )                    { m_bMassChanged = false; };
    void                        RestoreTurnMass                     ( void )                    { m_bTurnMassChanged = false; };
    void                        RestoreDragCoeff                    ( void )                    { m_bDragCoeffChanged = false; };
    void                        RestoreCenterOfMass                 ( void )                    { m_bCenterOfMassChanged = false; };

    void                        RestorePercentSubmerged             ( void )                    { m_bPercentSubmergedChanged = false; };
    void                        RestoreTractionMultiplier           ( void )                    { m_bTractionMultiplierChanged = false; };

    void                        RestoreDriveType                    ( void )                    { m_bDriveTypeChanged = false; };
    void                        RestoreEngineType                   ( void )                    { m_bEngineTypeChanged = false; };
    void                        RestoreNumberOfGears                ( void )                    { m_bNumberOfGearsChanged = false; };

    void                        RestoreEngineAccelleration          ( void )                    { m_bEngineAccellerationChanged = false; };
    void                        RestoreEngineInertia                ( void )                    { m_bEngineInertiaChanged = false; };
    void                        RestoreMaxVelocity                  ( void )                    { m_bMaxVelocityChanged = false; };
    
    void                        RestoreBrakeDecelleration           ( void )                    { m_bBrakeDecellerationChanged = false; };
    void                        RestoreBrakeBias                    ( void )                    { m_bBrakeBiasChanged = false; };
    void                        RestoreABS                          ( void )                    { m_bABSChanged = false; };

    void                        RestoreSteeringLock                 ( void )                    { m_bSteeringLockChanged = false; };
    void                        RestoreTractionLoss                 ( void )                    { m_bTractionLossChanged = false; };
    void                        RestoreTractionBias                 ( void )                    { m_bTractionBiasChanged = false; };

    void                        RestoreSuspensionForceLevel         ( void )                    { m_bSuspensionForceLevelChanged = false; };
    void                        RestoreSuspensionDamping            ( void )                    { m_bSuspensionDampingChanged = false; };
    void                        RestoreSuspensionHighSpeedDamping   ( void )                    { m_bSuspensionHighSpdDampingChanged = false; };
    void                        RestoreSuspensionUpperLimit         ( void )                    { m_bSuspensionUpperLimitChanged = false; };
    void                        RestoreSuspensionLowerLimit         ( void )                    { m_bSuspensionLowerLimitChanged = false; };
    void                        RestoreSuspensionFrontRearBias      ( void )                    { m_bSuspensionFrontRearBiasChanged = false; };
    void                        RestoreSuspensionAntidiveMultiplier ( void )                    { m_bSuspensionAntidiveMultiplierChanged = false; };

    void                        RestoreCollisionDamageMultiplier    ( void )                    { m_bCollisionDamageMultiplierChanged = false; };

    void                        RestoreHandlingFlags                ( void )                    { m_bHandlingFlagsChanged = false; };
    void                        RestoreModelFlags                   ( void )                    { m_bModelFlagsChanged = false; };
    void                        RestoreSeatOffsetDistance           ( void )                    { m_bSeatOffsetDistanceChanged = false; };

    void                        RestoreHeadLight                    ( void )                    { m_bHeadLightChanged = false; };
    void                        RestoreTailLight                    ( void )                    { m_bTailLightChanged = false; };
    void                        RestoreAnimGroup                    ( void )                    { m_bAnimGroupChanged = false; };

private:
                                CHandling                       ( class CHandlingManager* pHandlingManager );

    void                        CopyFrom                            ( const CHandling& Copy );
    void                        ResetChangedBools                   ( bool bValue );

    CHandlingManager*           m_pHandlingManager;

    std::list < unsigned short >m_DefaultTo;

    bool                        m_bMassChanged;
    float                       m_fMass;

    bool                        m_bTurnMassChanged;
    float                       m_fTurnMass;

    bool                        m_bDragCoeffChanged;
    float                       m_fDragCoeff;

    bool                        m_bCenterOfMassChanged;
    CVector                     m_vecCenterOfMass;

    bool                        m_bPercentSubmergedChanged;
    unsigned int                m_uiPercentSubmerged;

    bool                        m_bTractionMultiplierChanged;
    float                       m_fTractionMultiplier;

    bool                        m_bDriveTypeChanged;
    unsigned char               m_ucDriveType;

    bool                        m_bEngineTypeChanged;
    unsigned char               m_ucEngineType;

    bool                        m_bNumberOfGearsChanged;
    unsigned char               m_ucNumberOfGears;

    bool                        m_bEngineAccellerationChanged;
    float                       m_fEngineAccelleration;

    bool                        m_bEngineInertiaChanged;
    float                       m_fEngineInertia;

    bool                        m_bMaxVelocityChanged;
    float                       m_fMaxVelocity;

    bool                        m_bBrakeDecellerationChanged;
    float                       m_fBrakeDecelleration;

    bool                        m_bBrakeBiasChanged;
    float                       m_fBrakeBias;

    bool                        m_bABSChanged;
    bool                        m_bABS;

    bool                        m_bSteeringLockChanged;
    float                       m_fSteeringLock;

    bool                        m_bTractionLossChanged;
    float                       m_fTractionLoss;

    bool                        m_bTractionBiasChanged;
    float                       m_fTractionBias;

    bool                        m_bSuspensionForceLevelChanged;
    float                       m_fSuspensionForceLevel;

    bool                        m_bSuspensionDampingChanged;
    float                       m_fSuspensionDamping;

    bool                        m_bSuspensionHighSpdDampingChanged;
    float                       m_fSuspensionHighSpdDamping;

    bool                        m_bSuspensionUpperLimitChanged;
    float                       m_fSuspensionUpperLimit;

    bool                        m_bSuspensionLowerLimitChanged;
    float                       m_fSuspensionLowerLimit;

    bool                        m_bSuspensionFrontRearBiasChanged;
    float                       m_fSuspensionFrontRearBias;

    bool                        m_bSuspensionAntidiveMultiplierChanged;
    float                       m_fSuspensionAntidiveMultiplier;

    bool                        m_bCollisionDamageMultiplierChanged;
    float                       m_fCollisionDamageMultiplier;

    bool                        m_bModelFlagsChanged;
    unsigned int                m_uiModelFlags;

    bool                        m_bHandlingFlagsChanged;
    unsigned int                m_uiHandlingFlags;

    bool                        m_bSeatOffsetDistanceChanged;
    float                       m_fSeatOffsetDistance;

    bool                        m_bHeadLightChanged;
    unsigned char               m_ucHeadLight;

    bool                        m_bTailLightChanged;
    unsigned char               m_ucTailLight;

    bool                        m_bAnimGroupChanged;
    unsigned char               m_ucAnimGroup;
};

#endif
