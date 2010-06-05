/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHandling.cpp
*  PURPOSE:     Vehicle handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHandling::CHandling ( class CHandlingManager* pHandlingManager ): CElement ( NULL )
{
    // Init
    m_pHandlingManager = pHandlingManager;
    m_iType = CElement::HANDLING;
    SetTypeName ( "handling" );

    // Start with true (everything changed) for the default handlings created by the manager.
    ResetChangedBools ( true );
}


CHandling::CHandling ( CElement* pParent, CXMLNode* pNode, class CHandlingManager* pHandlingManager ) : CElement ( pParent, pNode )
{
    // Init
    m_pHandlingManager = pHandlingManager;
    m_iType = CElement::HANDLING;
    SetTypeName ( "handling" );

    // Nothing is changed for user created handlings, yet
    ResetChangedBools ( false );

    // Add us to the manager's list
    pHandlingManager->AddToList ( this );
}


CHandling::CHandling ( const CHandling& Copy ) : CElement ( Copy.m_pParent, Copy.m_pXMLNode )
{
    // Use its handling manager
    m_pHandlingManager = Copy.m_pHandlingManager;

    // Copy all the data from it
    CopyFrom ( Copy );

    // Add us to the manager's list
    m_pHandlingManager->AddToList ( this );
}


CHandling::~CHandling ( void )
{
    // Unlink us from manager
    Unlink ();
}


void CHandling::Unlink ( void )
{
    // Remove us from the manager's list
    m_pHandlingManager->RemoveFromList ( this );
}


bool CHandling::ReadSpecialData ( void )
{
    // Read out stuff from element data
    GetCustomDataFloat ( "mass", m_fMass, true );
    GetCustomDataFloat ( "turnMass", m_fTurnMass, true );
    GetCustomDataFloat ( "dragCoefficiency", m_fDragCoeff, true );
    GetCustomDataFloat ( "centerOfMassX", m_vecCenterOfMass.fX, true );
    GetCustomDataFloat ( "centerOfMassY", m_vecCenterOfMass.fY, true );
    GetCustomDataFloat ( "centerOfMassZ", m_vecCenterOfMass.fZ, true );

    // TODO: Rest... cba :)

    // Success
    return true;
}


void CHandling::AddDefaultTo ( unsigned short usID )
{
    // Add it to our list
    m_DefaultTo.push_front ( usID );
}


bool CHandling::IsDefaultFor ( unsigned short usID )
{
    // Loop through our list looking for it
    std::list < unsigned short > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        if ( *iter == usID )
            return true;
    }

    // Nop
    return false;
}


float CHandling::GetMass ( bool& bDefault )
{
    bDefault = m_bMassChanged;
    return m_fMass;
}


float CHandling::GetTurnMass ( bool& bDefault )
{
    bDefault = m_bTurnMassChanged;
    return m_fTurnMass;
}


float CHandling::GetDragCoeff ( bool& bDefault )
{
    bDefault = m_bDragCoeffChanged;
    return m_fDragCoeff;
}


const CVector& CHandling::GetCenterOfMass ( bool& bDefault )
{ 
    bDefault = m_bCenterOfMassChanged;
    return m_vecCenterOfMass; 
}


unsigned int CHandling::GetPercentSubmerged ( bool& bDefault )
{ 
    bDefault = m_bPercentSubmergedChanged;
    return m_uiPercentSubmerged; 
}


float CHandling::GetTractionMultiplier ( bool& bDefault )
{ 
    bDefault = m_bTractionMultiplierChanged;
    return m_fTractionMultiplier; 
}


CHandling::eDriveType CHandling::GetDriveType ( bool& bDefault )
{ 
    bDefault = m_bDriveTypeChanged;
    return static_cast < eDriveType > ( m_ucDriveType ); 
}


CHandling::eEngineType CHandling::GetEngineType ( bool& bDefault )
{ 
    bDefault = m_bEngineTypeChanged;
    return static_cast < eEngineType > ( m_ucEngineType ); 
}


unsigned char CHandling::GetNumberOfGears ( bool& bDefault )
{ 
    bDefault = m_bNumberOfGearsChanged;
    return m_ucNumberOfGears; 
}


float CHandling::GetEngineAccelleration ( bool& bDefault )
{ 
    bDefault = m_bEngineAccellerationChanged;
    return m_fEngineAccelleration; 
}


float CHandling::GetEngineInertia ( bool& bDefault )
{ 
    bDefault = m_bEngineInertiaChanged;
    return m_fEngineInertia; 
}


float CHandling::GetMaxVelocity ( bool& bDefault )
{ 
    bDefault = m_bMaxVelocityChanged;
    return m_fMaxVelocity; 
}


float CHandling::GetBrakeDecelleration ( bool& bDefault )
{ 
    bDefault = m_bBrakeDecellerationChanged;
    return m_fBrakeDecelleration; 
}


float CHandling::GetBrakeBias ( bool& bDefault )
{ 
    bDefault = m_bBrakeBiasChanged;
    return m_fBrakeBias; 
}


bool CHandling::GetABS ( bool& bDefault )
{ 
    bDefault = m_bABSChanged;
    return m_bABS; 
}


float CHandling::GetSteeringLock ( bool& bDefault )
{ 
    bDefault = m_bSteeringLockChanged;
    return m_fSteeringLock; 
}


float CHandling::GetTractionLoss ( bool& bDefault )
{ 
    bDefault = m_bTractionLossChanged;
    return m_fTractionLoss; 
}


float CHandling::GetTractionBias ( bool& bDefault )
{ 
    bDefault = m_bTractionBiasChanged;
    return m_fTractionBias; 
}


float CHandling::GetSuspensionForceLevel ( bool& bDefault )
{ 
    bDefault = m_bSuspensionForceLevelChanged;
    return m_fSuspensionForceLevel; 
}


float CHandling::GetSuspensionDamping ( bool& bDefault )
{ 
    bDefault = m_bSuspensionDampingChanged;
    return m_fSuspensionDamping; 
}


float CHandling::GetSuspensionHighSpeedDamping ( bool& bDefault )
{ 
    bDefault = m_bSuspensionHighSpdDampingChanged;
    return m_fSuspensionHighSpdDamping; 
}


float CHandling::GetSuspensionUpperLimit ( bool& bDefault )
{ 
    bDefault = m_bSuspensionUpperLimitChanged;
    return m_fSuspensionUpperLimit; 
}


float CHandling::GetSuspensionLowerLimit ( bool& bDefault )
{ 
    bDefault = m_bSuspensionLowerLimitChanged;
    return m_fSuspensionLowerLimit; 
}


float CHandling::GetSuspensionFrontRearBias ( bool& bDefault )
{ 
    bDefault = m_bSuspensionFrontRearBiasChanged;
    return m_fSuspensionFrontRearBias; 
}

float CHandling::GetSuspensionAntidiveMultiplier ( bool& bDefault )
{ 
    bDefault = m_bSuspensionAntidiveMultiplierChanged;
    return m_fSuspensionAntidiveMultiplier; 
}


float CHandling::GetCollisionDamageMultiplier ( bool& bDefault )
{ 
    bDefault = m_bCollisionDamageMultiplierChanged;
    return m_fCollisionDamageMultiplier; 
}


unsigned int CHandling::GetHandlingFlags ( bool& bDefault )
{ 
    bDefault = m_bHandlingFlagsChanged;
    return m_uiHandlingFlags; 
}


unsigned int CHandling::GetModelFlags ( bool& bDefault )
{ 
    bDefault = m_bModelFlagsChanged;
    return m_uiModelFlags; 
}


float CHandling::GetSeatOffsetDistance ( bool& bDefault )
{ 
    bDefault = m_bSeatOffsetDistanceChanged;
    return m_fSeatOffsetDistance; 
}


CHandling::eLightType CHandling::GetHeadLight ( bool& bDefault )
{ 
    bDefault = m_bHeadLightChanged;
    return static_cast < eLightType > ( m_ucHeadLight ); 
}


CHandling::eLightType CHandling::GetTailLight ( bool& bDefault )
{ 
    bDefault = m_bTailLightChanged;
    return static_cast < eLightType > ( m_ucTailLight ); 
}


unsigned char CHandling::GetAnimGroup ( bool& bDefault )
{ 
    bDefault = m_bAnimGroupChanged;
    return m_ucAnimGroup; 
}


void CHandling::CopyFrom ( const CHandling& Copy )
{
    // Copy properties
    m_fMass = Copy.m_fMass;
    m_fTurnMass = Copy.m_fTurnMass;
    m_fDragCoeff = Copy.m_fDragCoeff;
    m_vecCenterOfMass = Copy.m_vecCenterOfMass;

    m_uiPercentSubmerged = Copy.m_uiPercentSubmerged;
    m_fTractionMultiplier = Copy.m_fTractionMultiplier;

    m_ucDriveType = Copy.m_ucDriveType;
    m_ucEngineType = Copy.m_ucEngineType;
    m_ucNumberOfGears = Copy.m_ucNumberOfGears;

    m_fEngineAccelleration = Copy.m_fEngineAccelleration;
    m_fEngineInertia = Copy.m_fEngineInertia;
    m_fMaxVelocity = Copy.m_fMaxVelocity;

    m_fBrakeDecelleration = Copy.m_fBrakeDecelleration;
    m_fBrakeBias = Copy.m_fBrakeBias;
    m_bABS = Copy.m_bABS;

    m_fSteeringLock = Copy.m_fSteeringLock;
    m_fTractionLoss = Copy.m_fTractionLoss;
    m_fTractionBias = Copy.m_fTractionBias;

    m_fSuspensionForceLevel = Copy.m_fSuspensionForceLevel;
    m_fSuspensionDamping = Copy.m_fSuspensionDamping;
    m_fSuspensionHighSpdDamping = Copy.m_fSuspensionHighSpdDamping;
    m_fSuspensionUpperLimit = Copy.m_fSuspensionUpperLimit;
 
    m_fSuspensionLowerLimit = Copy.m_fSuspensionLowerLimit;
    m_fSuspensionFrontRearBias = Copy.m_fSuspensionFrontRearBias;
    m_fSuspensionAntidiveMultiplier = Copy.m_fSuspensionAntidiveMultiplier;

    m_fCollisionDamageMultiplier = Copy.m_fCollisionDamageMultiplier;

    m_uiModelFlags = Copy.m_uiModelFlags;
    m_uiHandlingFlags = Copy.m_uiHandlingFlags;
    m_fSeatOffsetDistance = Copy.m_fSeatOffsetDistance;

    m_ucHeadLight = Copy.m_ucHeadLight;
    m_ucTailLight = Copy.m_ucTailLight;
    m_ucAnimGroup = Copy.m_ucAnimGroup;


    // Copy changed flags
    m_bMassChanged = Copy.m_bMassChanged;
    m_bTurnMassChanged = Copy.m_bTurnMassChanged;
    m_bDragCoeffChanged = Copy.m_bDragCoeffChanged;
    m_bCenterOfMassChanged = Copy.m_bCenterOfMassChanged;
    m_bPercentSubmergedChanged = Copy.m_bPercentSubmergedChanged;
    m_bTractionMultiplierChanged = Copy.m_bTractionMultiplierChanged;
    m_bDriveTypeChanged = Copy.m_bDriveTypeChanged;
    m_bEngineTypeChanged = Copy.m_bEngineTypeChanged;
    m_bNumberOfGearsChanged = Copy.m_bNumberOfGearsChanged;
    m_bEngineAccellerationChanged = Copy.m_bEngineAccellerationChanged;
    m_bEngineInertiaChanged = Copy.m_bEngineInertiaChanged;
    m_bMaxVelocityChanged = Copy.m_bMaxVelocityChanged;
    m_bBrakeDecellerationChanged = Copy.m_bBrakeDecellerationChanged;
    m_bBrakeBiasChanged = Copy.m_bBrakeBiasChanged;
    m_bABSChanged = Copy.m_bABSChanged;
    m_bSteeringLockChanged = Copy.m_bSteeringLockChanged;
    m_bTractionLossChanged = Copy.m_bTractionLossChanged;
    m_bTractionBiasChanged = Copy.m_bTractionBiasChanged;
    m_bSuspensionForceLevelChanged = Copy.m_bSuspensionForceLevelChanged;
    m_bSuspensionDampingChanged = Copy.m_bSuspensionDampingChanged;
    m_bSuspensionHighSpdDampingChanged = Copy.m_bSuspensionHighSpdDampingChanged;
    m_bSuspensionUpperLimitChanged = Copy.m_bSuspensionUpperLimitChanged;
    m_bSuspensionLowerLimitChanged = Copy.m_bSuspensionLowerLimitChanged;
    m_bSuspensionFrontRearBiasChanged = Copy.m_bSuspensionFrontRearBiasChanged;
    m_bSuspensionAntidiveMultiplierChanged = Copy.m_bSuspensionAntidiveMultiplierChanged;
    m_bCollisionDamageMultiplierChanged = Copy.m_bCollisionDamageMultiplierChanged;
    m_bModelFlagsChanged = Copy.m_bModelFlagsChanged;
    m_bHandlingFlagsChanged = Copy.m_bHandlingFlagsChanged;
    m_bSeatOffsetDistanceChanged = Copy.m_bSeatOffsetDistanceChanged;
    m_bHeadLightChanged = Copy.m_bHeadLightChanged;
    m_bTailLightChanged = Copy.m_bTailLightChanged;
    m_bAnimGroupChanged = Copy.m_bAnimGroupChanged;
}


void CHandling::ResetChangedBools ( bool bValue )
{
    m_bMassChanged = bValue;
    m_bTurnMassChanged = bValue;
    m_bDragCoeffChanged = bValue;
    m_bCenterOfMassChanged = bValue;
    m_bPercentSubmergedChanged = bValue;
    m_bTractionMultiplierChanged = bValue;
    m_bDriveTypeChanged = bValue;
    m_bEngineTypeChanged = bValue;
    m_bNumberOfGearsChanged = bValue;
    m_bEngineAccellerationChanged = bValue;
    m_bEngineInertiaChanged = bValue;
    m_bMaxVelocityChanged = bValue;
    m_bBrakeDecellerationChanged = bValue;
    m_bBrakeBiasChanged = bValue;
    m_bABSChanged = bValue;
    m_bSteeringLockChanged = bValue;
    m_bTractionLossChanged = bValue;
    m_bTractionBiasChanged = bValue;
    m_bSuspensionForceLevelChanged = bValue;
    m_bSuspensionDampingChanged = bValue;
    m_bSuspensionHighSpdDampingChanged = bValue;
    m_bSuspensionUpperLimitChanged = bValue;
    m_bSuspensionLowerLimitChanged = bValue;
    m_bSuspensionFrontRearBiasChanged = bValue;
    m_bSuspensionAntidiveMultiplierChanged = bValue;
    m_bCollisionDamageMultiplierChanged = bValue;
    m_bModelFlagsChanged = bValue;
    m_bHandlingFlagsChanged = bValue;
    m_bSeatOffsetDistanceChanged = bValue;
    m_bHeadLightChanged = bValue;
    m_bTailLightChanged = bValue;
    m_bAnimGroupChanged = bValue;
}
