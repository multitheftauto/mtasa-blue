/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientHandling.h
*  PURPOSE:     Vehicle handling entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class CClientHandling;

#ifndef __CCLIENTHANDLING_H
#define __CCLIENTHANDLING_H

#include "CClientEntity.h"
#include <list>

class CClientHandling : public CClientEntity
{
    friend class CClientHandlingManager;

public:
                                    CClientHandling                 ( class CClientManager* pManager, ElementID ID );
                                    ~CClientHandling                ( void );

    // Element functions
    eClientEntityType               GetType                         ( void ) const                  { return CCLIENTHANDLING; };
    void                            Unlink                          ( void )                        {};

    void                            GetPosition                     ( CVector& vecPosition ) const  { vecPosition = CVector ( 0, 0, 0 ); };
    void                            SetPosition                     ( const CVector& vecPosition )  {};

    // Vehicles this handling is defaulted to
    size_t                          CountDefaultTo                  ( void )                { return m_DefaultTo.size (); };
    bool                            IsDefaultFor                    ( eVehicleTypes ID );
    std::list < eVehicleTypes >::const_iterator IterDefaultToBegin  ( void )                { return m_DefaultTo.begin (); };
    std::list < eVehicleTypes >::const_iterator IterDefaultToEnd    ( void )                { return m_DefaultTo.end (); };

    void                            AddDefaultTo                    ( eVehicleTypes ID );
    void                            RemoveDefaultTo                 ( eVehicleTypes ID );
    void                            ClearDefaultTo                  ( void );

    // Get functions
    float                           GetMass                         ( bool& bChanged );
    float                           GetTurnMass                     ( bool& bChanged );
    float                           GetDragCoeff                    ( bool& bChanged );
    const CVector&                  GetCenterOfMass                 ( bool& bChanged );

    unsigned int                    GetPercentSubmerged             ( bool& bChanged );
    float                           GetTractionMultiplier           ( bool& bChanged );

    CHandlingEntry::eDriveType      GetDriveType                    ( bool& bChanged );
    CHandlingEntry::eEngineType     GetEngineType                   ( bool& bChanged );
    unsigned char                   GetNumberOfGears                ( bool& bChanged );

    float                           GetEngineAccelleration          ( bool& bChanged );
    float                           GetEngineInertia                ( bool& bChanged );
    float                           GetMaxVelocity                  ( bool& bChanged );

    float                           GetBrakeDecelleration           ( bool& bChanged );
    float                           GetBrakeBias                    ( bool& bChanged );
    bool                            GetABS                          ( bool& bChanged );

    float                           GetSteeringLock                 ( bool& bChanged );
    float                           GetTractionLoss                 ( bool& bChanged );
    float                           GetTractionBias                 ( bool& bChanged );

    float                           GetSuspensionForceLevel         ( bool& bChanged );
    float                           GetSuspensionDamping            ( bool& bChanged );
    float                           GetSuspensionHighSpeedDamping   ( bool& bChanged );
    float                           GetSuspensionUpperLimit         ( bool& bChanged );
    float                           GetSuspensionLowerLimit         ( bool& bChanged );
    float                           GetSuspensionFrontRearBias      ( bool& bChanged );
    float                           GetSuspensionAntidiveMultiplier ( bool& bChanged );

    float                           GetCollisionDamageMultiplier    ( bool& bChanged );

    unsigned int                    GetHandlingFlags                ( bool& bChanged );
    unsigned int                    GetModelFlags                   ( bool& bChanged );
    float                           GetSeatOffsetDistance           ( bool& bChanged );

    CHandlingEntry::eLightType      GetHeadLight                    ( bool& bChanged );
    CHandlingEntry::eLightType      GetTailLight                    ( bool& bChanged );
    unsigned char                   GetAnimGroup                    ( bool& bChanged );


    // Set functions
    void                            SetMass                             ( float fMass, bool bRestore = false );
    void                            SetTurnMass                         ( float fTurnMass, bool bRestore = false );
    void                            SetDragCoeff                        ( float fDrag, bool bRestore = false );
    void                            SetCenterOfMass                     ( const CVector& vecCenter, bool bRestore = false );

    void                            SetPercentSubmerged                 ( unsigned int uiPercent, bool bRestore = false );
    void                            SetTractionMultiplier               ( float fTractionMultiplier, bool bRestore = false );

    void                            SetDriveType                        ( CHandlingEntry::eDriveType Type, bool bRestore = false );
    void                            SetEngineType                       ( CHandlingEntry::eEngineType Type, bool bRestore = false );
    void                            SetNumberOfGears                    ( unsigned char ucNumber, bool bRestore = false );

    void                            SetEngineAccelleration              ( float fAccelleration, bool bRestore = false );
    void                            SetEngineInertia                    ( float fInertia, bool bRestore = false );
    void                            SetMaxVelocity                      ( float fVelocity, bool bRestore = false );
    
    void                            SetBrakeDecelleration               ( float fDecelleration, bool bRestore = false );
    void                            SetBrakeBias                        ( float fBias, bool bRestore = false );
    void                            SetABS                              ( bool bABS, bool bRestore = false );

    void                            SetSteeringLock                     ( float fSteeringLock, bool bRestore = false );
    void                            SetTractionLoss                     ( float fTractionLoss, bool bRestore = false );
    void                            SetTractionBias                     ( float fTractionBias, bool bRestore = false );

    void                            SetSuspensionForceLevel             ( float fForce, bool bRestore = false );
    void                            SetSuspensionDamping                ( float fDamping, bool bRestore = false );
    void                            SetSuspensionHighSpeedDamping       ( float fDamping, bool bRestore = false );
    void                            SetSuspensionUpperLimit             ( float fUpperLimit, bool bRestore = false );
    void                            SetSuspensionLowerLimit             ( float fLowerLimit, bool bRestore = false );
    void                            SetSuspensionFrontRearBias          ( float fBias, bool bRestore = false );
    void                            SetSuspensionAntidiveMultiplier     ( float fAntidive, bool bRestore = false );

    void                            SetCollisionDamageMultiplier        ( float fMultiplier, bool bRestore = false );

    void                            SetHandlingFlags                    ( unsigned int uiFlags, bool bRestore = false );
    void                            SetModelFlags                       ( unsigned int uiFlags, bool bRestore = false );
    void                            SetSeatOffsetDistance               ( float fDistance, bool bRestore = false );

    void                            SetHeadLight                        ( CHandlingEntry::eLightType Style, bool bRestore = false );
    void                            SetTailLight                        ( CHandlingEntry::eLightType Style, bool bRestore = false );
    void                            SetAnimGroup                        ( unsigned char ucGroup, bool bRestore = false );

private:
                                    CClientHandling                     ( class CClientManager* pManager );

    void                            ResetChangedBools                   ( bool bValue );

    CClientHandlingManager*         m_pHandlingManager;
    std::list < eVehicleTypes >     m_DefaultTo;

    bool                            m_bMassChanged;
    float                           m_fMass;

    bool                            m_bTurnMassChanged;
    float                           m_fTurnMass;

    bool                            m_bDragCoeffChanged;
    float                           m_fDragCoeff;

    bool                            m_bCenterOfMassChanged;
    CVector                         m_vecCenterOfMass;

    bool                            m_bPercentSubmergedChanged;
    unsigned int                    m_uiPercentSubmerged;

    bool                            m_bTractionMultiplierChanged;
    float                           m_fTractionMultiplier;

    bool                            m_bDriveTypeChanged;
    unsigned char                   m_ucDriveType;

    bool                            m_bEngineTypeChanged;
    unsigned char                   m_ucEngineType;

    bool                            m_bNumberOfGearsChanged;
    unsigned char                   m_ucNumberOfGears;

    bool                            m_bEngineAccellerationChanged;
    float                           m_fEngineAccelleration;

    bool                            m_bEngineInertiaChanged;
    float                           m_fEngineInertia;

    bool                            m_bMaxVelocityChanged;
    float                           m_fMaxVelocity;

    bool                            m_bBrakeDecellerationChanged;
    float                           m_fBrakeDecelleration;

    bool                            m_bBrakeBiasChanged;
    float                           m_fBrakeBias;

    bool                            m_bABSChanged;
    bool                            m_bABS;

    bool                            m_bSteeringLockChanged;
    float                           m_fSteeringLock;

    bool                            m_bTractionLossChanged;
    float                           m_fTractionLoss;

    bool                            m_bTractionBiasChanged;
    float                           m_fTractionBias;

    bool                            m_bSuspensionForceLevelChanged;
    float                           m_fSuspensionForceLevel;

    bool                            m_bSuspensionDampingChanged;
    float                           m_fSuspensionDamping;

    bool                            m_bSuspensionHighSpdDampingChanged;
    float                           m_fSuspensionHighSpdDamping;

    bool                            m_bSuspensionUpperLimitChanged;
    float                           m_fSuspensionUpperLimit;

    bool                            m_bSuspensionLowerLimitChanged;
    float                           m_fSuspensionLowerLimit;

    bool                            m_bSuspensionFrontRearBiasChanged;
    float                           m_fSuspensionFrontRearBias;

    bool                            m_bSuspensionAntidiveMultiplierChanged;
    float                           m_fSuspensionAntidiveMultiplier;

    bool                            m_bCollisionDamageMultiplierChanged;
    float                           m_fCollisionDamageMultiplier;

    bool                            m_bModelFlagsChanged;
    unsigned int                    m_uiModelFlags;

    bool                            m_bHandlingFlagsChanged;
    unsigned int                    m_uiHandlingFlags;

    bool                            m_bSeatOffsetDistanceChanged;
    float                           m_fSeatOffsetDistance;

    bool                            m_bHeadLightChanged;
    unsigned char                   m_ucHeadLight;

    bool                            m_bTailLightChanged;
    unsigned char                   m_ucTailLight;

    bool                            m_bAnimGroupChanged;
    unsigned char                   m_ucAnimGroup;
};

#endif
