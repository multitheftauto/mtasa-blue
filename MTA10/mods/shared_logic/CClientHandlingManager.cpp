/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientHandlingManager.cpp
*  PURPOSE:     Vehicle handling entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

CClientHandlingManager::CClientHandlingManager ( class CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}


CClientHandlingManager::~CClientHandlingManager ( void )
{
    // Make sure all handlings are deleted
    DeleteAll ();
}


void CClientHandlingManager::DeleteAll ( void )
{
    // Make sure they don't remove themselves from the list
    m_bCanRemoveFromList = false;

    // Delete all handlings
    std::list < CClientHandling* > ::iterator iter = m_List.begin ();
    while ( iter != m_List.end () )
    {
        // Remove it from the list
        CClientHandling* pHandling = *iter;
        iter = m_List.erase ( iter );

        // Delete it
        delete pHandling;
    }

    m_bCanRemoveFromList = true;
}


CClientHandling* CClientHandlingManager::Get ( ElementID ID )
{
    // Grab it and check its type
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTHANDLING )
    {
        return static_cast < CClientHandling* > ( pEntity );
    }

    return NULL;
}


void  CClientHandlingManager::AddDefaultHandling ( eVehicleTypes eVehicle, CClientHandling* pHandling )
{
    // Add that handling as default for the given vehicle type
    pHandling->AddDefaultTo ( eVehicle );
}


void CClientHandlingManager::GetDefaultHandlings ( eVehicleTypes ID, std::vector < CClientHandling* >& List )
{
    // Loop through our list
    std::list < CClientHandling* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Is this default handling for that ID?
        if ( (*iter)->IsDefaultFor ( ID ) )
        {
            // Add it to the list
            List.push_back ( *iter );
        }
    }
}


void CClientHandlingManager::UpdateDefaultHandlings ( eVehicleTypes ID )
{
    // Grab the handling for this car
    CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetHandlingData ( ID );
    if ( pEntry )
    {
        // Grab the handlings attached to that car
        std::vector < CClientHandling* > List;
        GetDefaultHandlings ( ID, List );

        // Set all the properties
        pEntry->SetMass ( GetMass ( List, ID ) );
        pEntry->SetTurnMass ( GetTurnMass ( List, ID ) );
        pEntry->SetDragCoeff ( GetDragCoeff ( List, ID ) );
        pEntry->SetCenterOfMass ( GetCenterOfMass ( List, ID ) );
        pEntry->SetPercentSubmerged ( GetPercentSubmerged ( List, ID ) );
        pEntry->SetTractionMultiplier ( GetTractionMultiplier ( List, ID ) );
        pEntry->SetCarDriveType ( CClientHandlingManager::GetDriveType ( List, ID ) );
        pEntry->SetCarEngineType ( CClientHandlingManager::GetEngineType ( List, ID ) );
        pEntry->SetNumberOfGears ( GetNumberOfGears ( List, ID ) );
        pEntry->SetEngineAccelleration ( GetEngineAccelleration ( List, ID ) );
        pEntry->SetEngineInertia ( GetEngineInertia ( List, ID ) );
        pEntry->SetMaxVelocity ( GetMaxVelocity ( List, ID ) );
        pEntry->SetBrakeDecelleration ( GetBrakeDecelleration ( List, ID ) );
        pEntry->SetBrakeBias ( GetBrakeBias ( List, ID ) );
        pEntry->SetABS ( GetABS ( List, ID ) );
        pEntry->SetSteeringLock ( GetSteeringLock ( List, ID ) );
        pEntry->SetTractionLoss ( GetTractionLoss ( List, ID ) );
        pEntry->SetTractionBias ( GetTractionBias ( List, ID ) );
        pEntry->SetSuspensionForceLevel ( GetSuspensionForceLevel ( List, ID ) );
        pEntry->SetSuspensionDamping ( GetSuspensionDamping ( List, ID ) );
        pEntry->SetSuspensionHighSpeedDamping ( GetSuspensionHighSpeedDamping ( List, ID ) );
        pEntry->SetSuspensionUpperLimit ( GetSuspensionUpperLimit ( List, ID ) );
        pEntry->SetSuspensionLowerLimit ( GetSuspensionLowerLimit ( List, ID ) );
        pEntry->SetSuspensionFrontRearBias ( GetSuspensionFrontRearBias ( List, ID ) );
        pEntry->SetSuspensionAntidiveMultiplier ( GetSuspensionAntidiveMultiplier ( List, ID ) );
        pEntry->SetCollisionDamageMultiplier ( GetCollisionDamageMultiplier ( List, ID ) );
        pEntry->SetHandlingFlags ( GetHandlingFlags ( List, ID ) );
        pEntry->SetModelFlags ( GetModelFlags ( List, ID ) );
        pEntry->SetSeatOffsetDistance ( GetSeatOffsetDistance ( List, ID ) );
        pEntry->SetHeadLight ( GetHeadLight ( List, ID ) );
        pEntry->SetTailLight ( GetTailLight ( List, ID ) );
        pEntry->SetAnimGroup ( GetAnimGroup ( List, ID ) );

        // Recalculate it
        pEntry->Recalculate ();
    }
}


float CClientHandlingManager::GetMass ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetMass ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetMass ();
}


float CClientHandlingManager::GetTurnMass ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetTurnMass ( bChanged );
        if ( bChanged )
            return fFloat;

    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetTurnMass ();
}


float CClientHandlingManager::GetDragCoeff ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetDragCoeff ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetDragCoeff ();
}


const CVector& CClientHandlingManager::GetCenterOfMass ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    const CVector* pvecCenterOfMass;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        pvecCenterOfMass = &(*iter)->GetCenterOfMass ( bChanged );
        if ( bChanged )
            return *pvecCenterOfMass;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetCenterOfMass ();
}


unsigned int CClientHandlingManager::GetPercentSubmerged ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    unsigned int uiPercentSubmerged;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        uiPercentSubmerged = (*iter)->GetPercentSubmerged ( bChanged );
        if ( bChanged )
            return uiPercentSubmerged;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetPercentSubmerged ();
}


float CClientHandlingManager::GetTractionMultiplier ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetTractionMultiplier ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetTractionMultiplier ();
}


CHandlingEntry::eDriveType CClientHandlingManager::GetDriveType ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    CHandlingEntry::eDriveType eDrive;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        eDrive = (*iter)->GetDriveType ( bChanged );
        if ( bChanged )
            return eDrive;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetCarDriveType ();
}


CHandlingEntry::eEngineType CClientHandlingManager::GetEngineType ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    CHandlingEntry::eEngineType eEngine;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        eEngine = (*iter)->GetEngineType ( bChanged );
        if ( bChanged )
            return eEngine;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetCarEngineType ();
}


unsigned char CClientHandlingManager::GetNumberOfGears ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    unsigned char ucNumberOfGears;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        ucNumberOfGears = (*iter)->GetNumberOfGears ( bChanged );
        if ( bChanged )
            return ucNumberOfGears;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetNumberOfGears ();
}


float CClientHandlingManager::GetEngineAccelleration ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetEngineAccelleration ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetEngineAccelleration ();
}


float CClientHandlingManager::GetEngineInertia ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetEngineInertia ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetEngineInertia ();
}


float CClientHandlingManager::GetMaxVelocity ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetMaxVelocity ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetMaxVelocity ();
}


float CClientHandlingManager::GetBrakeDecelleration ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetBrakeDecelleration ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetBrakeDecelleration ();
}


float CClientHandlingManager::GetBrakeBias ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetBrakeBias ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetBrakeBias ();
}


bool CClientHandlingManager::GetABS ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    bool fABS;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fABS = (*iter)->GetABS ( bChanged );
        if ( bChanged )
            return fABS;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetABS ();
}


float CClientHandlingManager::GetSteeringLock ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSteeringLock ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSteeringLock ();
}


float CClientHandlingManager::GetTractionLoss ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetTractionLoss ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetTractionLoss ();
}


float CClientHandlingManager::GetTractionBias ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetTractionBias ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetTractionBias ();
}


float CClientHandlingManager::GetSuspensionForceLevel ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionForceLevel ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionForceLevel ();
}


float CClientHandlingManager::GetSuspensionDamping ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionDamping ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionDamping ();
}


float CClientHandlingManager::GetSuspensionHighSpeedDamping ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionHighSpeedDamping ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionHighSpeedDamping ();
}


float CClientHandlingManager::GetSuspensionUpperLimit ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionUpperLimit ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionUpperLimit ();
}


float CClientHandlingManager::GetSuspensionLowerLimit ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionLowerLimit ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionLowerLimit ();
}


float CClientHandlingManager::GetSuspensionFrontRearBias ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionFrontRearBias ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionFrontRearBias ();
}


float CClientHandlingManager::GetSuspensionAntidiveMultiplier ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSuspensionAntidiveMultiplier ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSuspensionAntidiveMultiplier ();
}


float CClientHandlingManager::GetCollisionDamageMultiplier ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetCollisionDamageMultiplier ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetCollisionDamageMultiplier ();
}


unsigned int CClientHandlingManager::GetHandlingFlags ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    unsigned int uiHandling;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        uiHandling = (*iter)->GetHandlingFlags ( bChanged );
        if ( bChanged )
            return uiHandling;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetHandlingFlags ();
}


unsigned int CClientHandlingManager::GetModelFlags ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    unsigned int uiHandling;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        uiHandling = (*iter)->GetModelFlags ( bChanged );
        if ( bChanged )
            return uiHandling;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetModelFlags ();
}


float CClientHandlingManager::GetSeatOffsetDistance ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    float fFloat;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        fFloat = (*iter)->GetSeatOffsetDistance ( bChanged );
        if ( bChanged )
            return fFloat;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetSeatOffsetDistance ();
}


CHandlingEntry::eLightType CClientHandlingManager::GetHeadLight ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    CHandlingEntry::eLightType eLight;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        eLight = (*iter)->GetHeadLight ( bChanged );
        if ( bChanged )
            return eLight;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetHeadLight ();
}


CHandlingEntry::eLightType CClientHandlingManager::GetTailLight ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    CHandlingEntry::eLightType eLight;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        eLight = (*iter)->GetTailLight ( bChanged );
        if ( bChanged )
            return eLight;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetTailLight ();
}


unsigned char CClientHandlingManager::GetAnimGroup ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID )
{
    // Look for the first entry in the list that's not default
    bool bChanged;
    unsigned char ucAnimGroup;
    std::vector < CClientHandling* > ::const_iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        ucAnimGroup = (*iter)->GetAnimGroup ( bChanged );
        if ( bChanged )
            return ucAnimGroup;
    }

    // Return the default
    return g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( DefaultValueID )->GetAnimGroup ();
}


void CClientHandlingManager::RemoveFromList ( CClientHandling* pHandling )
{
    if ( m_bCanRemoveFromList )
    {
        m_List.remove ( pHandling );
    }
}