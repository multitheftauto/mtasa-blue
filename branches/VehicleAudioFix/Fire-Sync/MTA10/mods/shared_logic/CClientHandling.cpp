/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientHandling.cpp
*  PURPOSE:     Vehicle handling entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

CClientHandling::CClientHandling ( CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pHandlingManager = pManager->GetHandlingManager ();
    SetTypeName ( "handling" );

    // Add to manager's list
    m_pHandlingManager->AddToList ( this );

    // Reset the changed bools to false. All start at default.
    ResetChangedBools ( false );
}


CClientHandling::~CClientHandling ( void )
{
    // Restore all vehicles we're default handling for
    ClearDefaultTo ();

    // Remove from manager's list
    m_pHandlingManager->RemoveFromList ( this );
}


bool CClientHandling::IsDefaultFor ( eVehicleTypes eVehicle )
{
    // Look for it in the list
    std::list < eVehicleTypes > ::iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        if ( *iter == eVehicle )
        {
            return true;
        }
    }

    // Nope
    return false;
}


void CClientHandling::AddDefaultTo ( eVehicleTypes eVehicle )
{
    // Already default for it? Don't add us twice.
    if ( IsDefaultFor ( eVehicle ) )
        return;

    // Add it to our list
    m_DefaultTo.push_front ( eVehicle );

    // Update handling for that vehicle ID. All values need to be
    // regenerated because they may not be correct anymore.
    m_pHandlingManager->UpdateDefaultHandlings ( eVehicle );
}


void CClientHandling::RemoveDefaultTo ( eVehicleTypes eVehicle )
{
    // Look for that value in the list and remove it
    bool bRemoved = false;
    std::list < eVehicleTypes > ::iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        if ( *iter == eVehicle )
        {
            m_DefaultTo.erase ( iter );
            bRemoved = true;
            break;
        }
    }

    // Removed something?
    if ( bRemoved )
    {
        // Update handling for that vehicle ID. All values need to be
        // regenerated because they may not be correct anymore.
        m_pHandlingManager->UpdateDefaultHandlings ( eVehicle );
    }
}


void CClientHandling::ClearDefaultTo ( void )
{
    // Loop through the list over vehicle model's we're default to
    eVehicleTypes eVehicle;
    std::list < eVehicleTypes > ::iterator iter = m_DefaultTo.begin ();
    while ( iter != m_DefaultTo.end () )
    {
        // Grab the ID and remove it from our list FIRST so that updating the
        // handling stuff won't recognize us as being default to that model.
        eVehicle = *iter;
        iter = m_DefaultTo.erase ( iter );

        // Update handling for that vehicle ID. All values need to be
        // regenerated because they may not be correct anymore.
        m_pHandlingManager->UpdateDefaultHandlings ( eVehicle );
    }
}


float CClientHandling::GetMass ( bool& bDefault )
{
    bDefault = m_bMassChanged;
    return m_fMass;
}


float CClientHandling::GetTurnMass ( bool& bDefault )
{
    bDefault = m_bTurnMassChanged;
    return m_fTurnMass;
}


float CClientHandling::GetDragCoeff ( bool& bDefault )
{
    bDefault = m_bDragCoeffChanged;
    return m_fDragCoeff;
}


const CVector& CClientHandling::GetCenterOfMass ( bool& bDefault )
{ 
    bDefault = m_bCenterOfMassChanged;
    return m_vecCenterOfMass; 
}


unsigned int CClientHandling::GetPercentSubmerged ( bool& bDefault )
{ 
    bDefault = m_bPercentSubmergedChanged;
    return m_uiPercentSubmerged; 
}


float CClientHandling::GetTractionMultiplier ( bool& bDefault )
{ 
    bDefault = m_bTractionMultiplierChanged;
    return m_fTractionMultiplier; 
}


CHandlingEntry::eDriveType CClientHandling::GetDriveType ( bool& bDefault )
{ 
    bDefault = m_bDriveTypeChanged;
    return static_cast < CHandlingEntry::eDriveType > ( m_ucDriveType ); 
}


CHandlingEntry::eEngineType CClientHandling::GetEngineType ( bool& bDefault )
{ 
    bDefault = m_bEngineTypeChanged;
    return static_cast < CHandlingEntry::eEngineType > ( m_ucEngineType ); 
}


unsigned char CClientHandling::GetNumberOfGears ( bool& bDefault )
{ 
    bDefault = m_bNumberOfGearsChanged;
    return m_ucNumberOfGears; 
}


float CClientHandling::GetEngineAccelleration ( bool& bDefault )
{ 
    bDefault = m_bEngineAccellerationChanged;
    return m_fEngineAccelleration; 
}


float CClientHandling::GetEngineInertia ( bool& bDefault )
{ 
    bDefault = m_bEngineInertiaChanged;
    return m_fEngineInertia; 
}


float CClientHandling::GetMaxVelocity ( bool& bDefault )
{ 
    bDefault = m_bMaxVelocityChanged;
    return m_fMaxVelocity; 
}


float CClientHandling::GetBrakeDecelleration ( bool& bDefault )
{ 
    bDefault = m_bBrakeDecellerationChanged;
    return m_fBrakeDecelleration; 
}


float CClientHandling::GetBrakeBias ( bool& bDefault )
{ 
    bDefault = m_bBrakeBiasChanged;
    return m_fBrakeBias; 
}


bool CClientHandling::GetABS ( bool& bDefault )
{ 
    bDefault = m_bABSChanged;
    return m_bABS; 
}


float CClientHandling::GetSteeringLock ( bool& bDefault )
{ 
    bDefault = m_bSteeringLockChanged;
    return m_fSteeringLock; 
}


float CClientHandling::GetTractionLoss ( bool& bDefault )
{ 
    bDefault = m_bTractionLossChanged;
    return m_fTractionLoss; 
}


float CClientHandling::GetTractionBias ( bool& bDefault )
{ 
    bDefault = m_bTractionBiasChanged;
    return m_fTractionBias; 
}


float CClientHandling::GetSuspensionForceLevel ( bool& bDefault )
{ 
    bDefault = m_bSuspensionForceLevelChanged;
    return m_fSuspensionForceLevel; 
}


float CClientHandling::GetSuspensionDamping ( bool& bDefault )
{ 
    bDefault = m_bSuspensionDampingChanged;
    return m_fSuspensionDamping; 
}


float CClientHandling::GetSuspensionHighSpeedDamping ( bool& bDefault )
{ 
    bDefault = m_bSuspensionHighSpdDampingChanged;
    return m_fSuspensionHighSpdDamping; 
}


float CClientHandling::GetSuspensionUpperLimit ( bool& bDefault )
{ 
    bDefault = m_bSuspensionUpperLimitChanged;
    return m_fSuspensionUpperLimit; 
}


float CClientHandling::GetSuspensionLowerLimit ( bool& bDefault )
{ 
    bDefault = m_bSuspensionLowerLimitChanged;
    return m_fSuspensionLowerLimit; 
}


float CClientHandling::GetSuspensionFrontRearBias ( bool& bDefault )
{ 
    bDefault = m_bSuspensionFrontRearBiasChanged;
    return m_fSuspensionFrontRearBias; 
}

float CClientHandling::GetSuspensionAntidiveMultiplier ( bool& bDefault )
{ 
    bDefault = m_bSuspensionAntidiveMultiplierChanged;
    return m_fSuspensionAntidiveMultiplier; 
}


float CClientHandling::GetCollisionDamageMultiplier ( bool& bDefault )
{ 
    bDefault = m_bCollisionDamageMultiplierChanged;
    return m_fCollisionDamageMultiplier; 
}


unsigned int CClientHandling::GetHandlingFlags ( bool& bDefault )
{ 
    bDefault = m_bHandlingFlagsChanged;
    return m_uiHandlingFlags; 
}


unsigned int CClientHandling::GetModelFlags ( bool& bDefault )
{ 
    bDefault = m_bModelFlagsChanged;
    return m_uiModelFlags; 
}


float CClientHandling::GetSeatOffsetDistance ( bool& bDefault )
{ 
    bDefault = m_bSeatOffsetDistanceChanged;
    return m_fSeatOffsetDistance; 
}


CHandlingEntry::eLightType CClientHandling::GetHeadLight ( bool& bDefault )
{ 
    bDefault = m_bHeadLightChanged;
    return static_cast < CHandlingEntry::eLightType > ( m_ucHeadLight ); 
}


CHandlingEntry::eLightType CClientHandling::GetTailLight ( bool& bDefault )
{ 
    bDefault = m_bTailLightChanged;
    return static_cast < CHandlingEntry::eLightType > ( m_ucTailLight ); 
}


unsigned char CClientHandling::GetAnimGroup ( bool& bDefault )
{ 
    bDefault = m_bAnimGroupChanged;
    return m_ucAnimGroup; 
}


void CClientHandling::SetMass ( float fMass, bool bRestore )
{
    // Set the property
    m_fMass = fMass;
    m_bMassChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetMass ( m_pHandlingManager->GetMass ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetTurnMass ( float fTurnMass, bool bRestore )
{ 
    // Set the property
    m_fTurnMass = fTurnMass;
    m_bTurnMassChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetTurnMass ( m_pHandlingManager->GetTurnMass ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetDragCoeff ( float fDrag, bool bRestore )
{ 
    // Set the property
    m_fDragCoeff = fDrag;
    m_bDragCoeffChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetDragCoeff ( m_pHandlingManager->GetDragCoeff ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetCenterOfMass ( const CVector& vecCenter, bool bRestore )
{ 
    // Set the property
    m_vecCenterOfMass = vecCenter;
    m_bCenterOfMassChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetCenterOfMass ( m_pHandlingManager->GetCenterOfMass ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetPercentSubmerged ( unsigned int uiPercent, bool bRestore )
{ 
    // Set the property
    m_uiPercentSubmerged = uiPercent;
    m_bPercentSubmergedChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetPercentSubmerged ( m_pHandlingManager->GetPercentSubmerged ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetTractionMultiplier ( float fTractionMultiplier, bool bRestore )
{ 
    // Set the property
    m_fTractionMultiplier = fTractionMultiplier;
    m_bTractionMultiplierChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetTractionMultiplier ( m_pHandlingManager->GetTractionMultiplier ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetDriveType ( CHandlingEntry::eDriveType Type, bool bRestore )
{ 
    // Set the property
    m_ucDriveType = Type;
    m_bDriveTypeChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetCarDriveType ( m_pHandlingManager->GetDriveType ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetEngineType ( CHandlingEntry::eEngineType Type, bool bRestore )
{ 
    // Set the property
    m_ucEngineType = Type;
    m_bEngineTypeChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetCarEngineType ( m_pHandlingManager->GetEngineType ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetNumberOfGears ( unsigned char ucNumber, bool bRestore )
{ 
    // Set the property
    m_ucNumberOfGears = ucNumber;
    m_bNumberOfGearsChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetNumberOfGears ( m_pHandlingManager->GetNumberOfGears ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetEngineAccelleration ( float fAccelleration, bool bRestore )
{
    // Set the property
    m_fEngineAccelleration = fAccelleration;
    m_bEngineAccellerationChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetEngineAccelleration ( m_pHandlingManager->GetEngineAccelleration ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetEngineInertia ( float fInertia, bool bRestore )
{ 
    // Set the property
    m_fEngineInertia = fInertia;
    m_bEngineInertiaChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetEngineInertia ( m_pHandlingManager->GetEngineInertia ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetMaxVelocity ( float fVelocity, bool bRestore )
{ 
    // Set the property
    m_fMaxVelocity = fVelocity;
    m_bMaxVelocityChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetMaxVelocity ( m_pHandlingManager->GetMaxVelocity ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetBrakeDecelleration ( float fDecelleration, bool bRestore )
{ 
    // Set the property
    m_fBrakeDecelleration = fDecelleration;
    m_bBrakeDecellerationChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetBrakeDecelleration ( m_pHandlingManager->GetBrakeDecelleration ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetBrakeBias ( float fBias, bool bRestore )
{ 
    // Set the property
    m_fBrakeBias = fBias;
    m_bBrakeBiasChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetBrakeBias ( m_pHandlingManager->GetBrakeBias ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetABS ( bool bABS, bool bRestore )
{ 
    // Set the property
    m_bABS = bABS;
    m_bABSChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetABS ( m_pHandlingManager->GetABS ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSteeringLock ( float fSteeringLock, bool bRestore )
{ 
    // Set the property
    m_fSteeringLock = fSteeringLock;
    m_bSteeringLockChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSteeringLock ( m_pHandlingManager->GetSteeringLock ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetTractionLoss ( float fTractionLoss, bool bRestore )
{ 
    // Set the property
    m_fTractionLoss = fTractionLoss;
    m_bTractionLossChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetTractionLoss ( m_pHandlingManager->GetTractionLoss ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}

void CClientHandling::SetTractionBias ( float fTractionBias, bool bRestore )    
{ 
    // Set the property
    m_fTractionBias = fTractionBias;
    m_bTractionBiasChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetTractionBias ( m_pHandlingManager->GetTractionBias ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionForceLevel ( float fForce, bool bRestore )
{ 
    // Set the property
    m_fSuspensionForceLevel = fForce;
    m_bSuspensionForceLevelChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionForceLevel ( m_pHandlingManager->GetSuspensionForceLevel ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionDamping ( float fDamping, bool bRestore )
{ 
    // Set the property
    m_fSuspensionDamping = fDamping;
    m_bSuspensionDampingChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionDamping ( m_pHandlingManager->GetSuspensionDamping ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionHighSpeedDamping ( float fDamping, bool bRestore )
{ 
    // Set the property
    m_fSuspensionHighSpdDamping = fDamping;
    m_bSuspensionHighSpdDampingChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionHighSpeedDamping ( m_pHandlingManager->GetSuspensionHighSpeedDamping ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionUpperLimit ( float fUpperLimit, bool bRestore )
{ 
    // Set the property
    m_fSuspensionUpperLimit = fUpperLimit; 
    m_bSuspensionUpperLimitChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionUpperLimit ( m_pHandlingManager->GetSuspensionUpperLimit ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionLowerLimit ( float fLowerLimit, bool bRestore )
{ 
    // Set the property
    m_fSuspensionLowerLimit = fLowerLimit;
    m_bSuspensionLowerLimitChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionLowerLimit ( m_pHandlingManager->GetSuspensionLowerLimit ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionFrontRearBias ( float fBias, bool bRestore )
{ 
    // Set the property
    m_fSuspensionFrontRearBias = fBias;
    m_bSuspensionFrontRearBiasChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionFrontRearBias ( m_pHandlingManager->GetSuspensionFrontRearBias ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSuspensionAntidiveMultiplier ( float fAntidive, bool bRestore )
{ 
    // Set the property
    m_fSuspensionAntidiveMultiplier = fAntidive;
    m_bSuspensionAntidiveMultiplierChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSuspensionAntidiveMultiplier ( m_pHandlingManager->GetSuspensionAntidiveMultiplier ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetCollisionDamageMultiplier ( float fMultiplier, bool bRestore )
{ 
    // Set the property
    m_fCollisionDamageMultiplier = fMultiplier;
    m_bCollisionDamageMultiplierChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetCollisionDamageMultiplier ( m_pHandlingManager->GetCollisionDamageMultiplier ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetHandlingFlags ( unsigned int uiFlags, bool bRestore )
{ 
    // Set the property
    m_uiHandlingFlags = uiFlags;
    m_bHandlingFlagsChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetHandlingFlags ( m_pHandlingManager->GetHandlingFlags ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetModelFlags ( unsigned int uiFlags, bool bRestore )
{ 
    // Set the property
    m_uiModelFlags = uiFlags;
    m_bModelFlagsChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetModelFlags ( m_pHandlingManager->GetModelFlags ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetSeatOffsetDistance ( float fDistance, bool bRestore )
{
    // Set the property
    m_fSeatOffsetDistance = fDistance;
    m_bSeatOffsetDistanceChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetSeatOffsetDistance ( m_pHandlingManager->GetSeatOffsetDistance ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetHeadLight ( CHandlingEntry::eLightType Style, bool bRestore )
{ 
    // Set the property
    m_ucHeadLight = Style;
    m_bHeadLightChanged = !bRestore; 

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetHeadLight ( m_pHandlingManager->GetHeadLight ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetTailLight ( CHandlingEntry::eLightType Style, bool bRestore )
{ 
    // Set the property
    m_ucTailLight = Style;
    m_bTailLightChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetTailLight ( m_pHandlingManager->GetTailLight ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::SetAnimGroup ( unsigned char ucGroup, bool bRestore )
{ 
    // Set the property
    m_ucAnimGroup = ucGroup;
    m_bAnimGroupChanged = !bRestore;

    // Loop through the vehicles we're default for
    std::list < eVehicleTypes > ::const_iterator iter = m_DefaultTo.begin ();
    for ( ; iter != m_DefaultTo.end (); iter++ )
    {
        // Grab the handling for this car
        CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( *iter );
        if ( pEntry )
        {
            // Grab the handlings attached to that car
            std::vector < CClientHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( *iter, List );

            // Update the property for this vehicle kind
            pEntry->SetAnimGroup ( m_pHandlingManager->GetAnimGroup ( List, *iter ) );
            pEntry->Recalculate ();
        }
    }
}


void CClientHandling::ResetChangedBools ( bool bValue )
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
