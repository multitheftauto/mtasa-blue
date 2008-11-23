/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicleManager.cpp
*  PURPOSE:     Vehicle entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

// List over all vehicles with their passenger max counts
unsigned char g_ucMaxPassengers [] = { 3, 1, 1, 1, 3, 3, 0, 1, 1, 3, 1, 1, 1, 3, 1, 1,              // 400->415
                                       3, 1, 3, 1, 3, 3, 1, 1, 1, 0, 3, 3, 3, 1, 0, 8,              // 416->431
                                       0, 1, 1, 255, 1, 8, 3, 1, 3, 0, 1, 1, 1, 3, 0, 1,            // 432->447
                                       0, 255, 255, 1, 0, 0, 0, 1, 1, 1, 3, 3, 1, 1, 1,
                                       1, 1, 1, 3, 3, 1, 1, 3, 1, 0, 0, 1, 1, 0, 1, 1,
                                       3, 1, 0, 3, 1, 0, 0, 0, 3, 1, 1, 3, 1, 3, 0, 1,
                                       1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 0, 0,
                                       1, 0, 0, 1, 1, 3, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
                                       1, 1, 3, 0, 0, 0, 1, 1, 1, 1, 255, 255, 0, 3, 1,
                                       1, 1, 1, 1, 3, 3, 1, 1, 3, 3, 1, 0, 1, 1, 1, 1,
                                       1, 1, 3, 3, 1, 1, 0, 1, 3, 3, 0, 255, 255, 0, 0,
                                       1, 0, 1, 1, 1, 1, 3, 3, 1, 3, 0, 255, 3, 1, 1, 1,
                                       1, 255, 255, 1, 1, 1, 0, 3, 3, 3, 1, 1, 1, 1, 1,
                                       3, 1, 255, 255, 255, 3, 255 };

// List over all vehicles with their special attributes
#define VEHICLE_HAS_TURRENT             0x001UL //1
#define VEHICLE_HAS_SIRENS              0x002UL //2
#define VEHICLE_HAS_LANDING_GEARS       0x004UL //4
#define VEHICLE_HAS_ADJUSTABLE_PROPERTY 0x008UL //8
#define VEHICLE_HAS_SMOKE_TRAIL         0x010UL //16
unsigned long g_ulVehicleAttributes [] = {
  0, 0, 0, 0, 0, 0, 8, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0,    // 400-424
  0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,    // 425-449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 450-474
  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 475-499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 4, 12, 0, 0, 2, 8, // 500-524
  8, 0, 0, 2, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,    // 525-549
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 550-574
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 2, 2, 2, 2,   // 575-599
  0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

CClientVehicleManager::CClientVehicleManager ( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}


CClientVehicleManager::~CClientVehicleManager ( void )
{
    // Destroy all vehicles
    DeleteAll ();
}


void CClientVehicleManager::DeleteAll ( void )
{
    // Delete all the vehicles
    m_bCanRemoveFromList = false;
    list < CClientVehicle* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
    m_bCanRemoveFromList = true;
}



void CClientVehicleManager::DoPulse ( void )
{
    CClientVehicle * pVehicle = NULL;
    // Loop through our streamed-in vehicles
    list < CClientVehicle * > cloneList = m_StreamedIn;
    list < CClientVehicle* > ::iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); iter++ )
    {
        pVehicle = *iter;
        // We should have a game vehicle here
        assert ( pVehicle->GetGameVehicle () );
        pVehicle->StreamedInPulse ();
    }
}


CClientVehicle* CClientVehicleManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTVEHICLE )
    {
        return static_cast < CClientVehicle* > ( pEntity );
    }

    return NULL;
}


CClientVehicle* CClientVehicleManager::Get ( CVehicle* pVehicle, bool bValidatePointer )
{
    if ( !pVehicle ) return NULL;

    if ( bValidatePointer )
    {
        list < CClientVehicle* > ::const_iterator iter = m_StreamedIn.begin ();
        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGameVehicle () == pVehicle )
            {
                return *iter;
            }
        }
    }
    else
    {
        return reinterpret_cast < CClientVehicle* > ( pVehicle->GetStoredPointer () );
    }
    return NULL;
}


CClientVehicle* CClientVehicleManager::GetSafe ( CEntity * pEntity )
{
    if ( !pEntity ) return NULL;


    list < CClientVehicle* > ::const_iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end (); iter++ )
    {
        if ( dynamic_cast < CEntity * > ( (*iter)->GetGameVehicle () ) == pEntity )
        {
            return *iter;
        }
    }

    return NULL;
}


CClientVehicle* CClientVehicleManager::GetClosest ( CVector& vecPosition, float fRadius )
{
    float fClosestDistance = 0.0f;
    CVector vecVehiclePosition;
    CClientVehicle* pClosest = NULL;
    list < CClientVehicle* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        (*iter)->GetPosition ( vecVehiclePosition );
        float fDistance = DistanceBetweenPoints3D ( vecPosition, vecVehiclePosition );
        if ( fDistance <= fRadius )
        {
            if ( pClosest == NULL || fDistance < fClosestDistance )
            {
                pClosest = *iter;
                fClosestDistance = fDistance;
            }
        }
    }
    return pClosest;
}


bool CClientVehicleManager::IsTrainModel ( unsigned long ulModel )
{
    return ( ulModel == 449 || ulModel == 537 || 
             ulModel == 538 || ulModel == 569 || 
             ulModel == 590 );
}


bool CClientVehicleManager::IsValidModel ( unsigned long ulModel )
{
    return ulModel >= 400 && ulModel <= 611;
}


eClientVehicleType CClientVehicleManager::GetVehicleType ( unsigned long ulModel )
{
    // Valid vehicle id?
    if ( IsValidModel ( ulModel ) )
    {
        // Grab the model info for the current vehicle
        CModelInfo* pModelInfo = g_pGame->GetModelInfo ( ulModel );
        if ( pModelInfo )
        {
            // Return the appropriate type
            if ( pModelInfo->IsCar () ) return CLIENTVEHICLE_CAR;
            if ( pModelInfo->IsBike () ) return CLIENTVEHICLE_BIKE;
            if ( pModelInfo->IsPlane () ) return CLIENTVEHICLE_PLANE;
            if ( pModelInfo->IsHeli () ) return CLIENTVEHICLE_HELI;
            if ( pModelInfo->IsBoat () ) return CLIENTVEHICLE_BOAT;
            if ( pModelInfo->IsQuadBike () ) return CLIENTVEHICLE_QUADBIKE;
            if ( pModelInfo->IsBmx () ) return CLIENTVEHICLE_BMX;
            if ( pModelInfo->IsMonsterTruck () ) return CLIENTVEHICLE_MONSTERTRUCK;
            if ( pModelInfo->IsTrailer () ) return CLIENTVEHICLE_TRAILER;
            if ( pModelInfo->IsTrain () ) return CLIENTVEHICLE_TRAIN;
        }
    }

    // Invalid vehicle id or some other error
    return CLIENTVEHICLE_NONE;
}


unsigned char CClientVehicleManager::GetMaxPassengerCount ( unsigned long ulModel )
{
    // Valid range?
    if ( ulModel >= 400 && ulModel <= 611 )
    {
        return g_ucMaxPassengers [ulModel - 400];
    }

    // Invalid index
    return 0xFF;
}


unsigned char CClientVehicleManager::ConvertIndexToGameSeat ( unsigned long ulModel, unsigned char ucIndex )
{
    eClientVehicleType vehicleType = GetVehicleType ( ulModel );
                
    // Grab the max passenger count for the given ID
    unsigned char ucMaxPassengerCount = GetMaxPassengerCount ( ulModel );
    switch ( ucMaxPassengerCount )
    {
        // Not passenger seats in this vehicle?
        case 0:
        case 255:
        {
            if ( ucIndex == 0 )
            {
                return DOOR_FRONT_LEFT;
            }
            
            return 0xFF;
        }

        // Only one seat?
        case 1:
        {
            bool bIsBike = ( vehicleType == CLIENTVEHICLE_BIKE ||
                             vehicleType == CLIENTVEHICLE_QUADBIKE );
            if ( ucIndex == 0 )
            {
                return DOOR_FRONT_LEFT;
            }
            else if ( ucIndex == 1 )
            {
                // We use one of the rear seats for bike passengers
                if ( bIsBike )
                {
                    return DOOR_REAR_RIGHT;
                }
                
                return DOOR_FRONT_RIGHT;
            }
            else if ( bIsBike )
            {
                switch ( ucIndex )
                {                    
                    case 2: return DOOR_REAR_LEFT;
                    case 3: return DOOR_REAR_RIGHT;
                }
            }
        
            return 0xFF;
        }

        // Three seats?
        case 3:
        {
            switch ( ucIndex )
            {
                case 0: return DOOR_FRONT_LEFT;                
                case 1: return DOOR_FRONT_RIGHT;
                case 2: return DOOR_REAR_LEFT;
                case 3: return DOOR_REAR_RIGHT;
            }

            return 0xFF;
        }

        // Bus?
        case 8:
        {
            if ( ucIndex == 0 )
            {
                return DOOR_FRONT_LEFT;
            }
            
            if ( ucIndex <= 8 )
            {
                return DOOR_FRONT_RIGHT;
            }

            return 0xFF;
        }
    }

    return 0xFF;
}


bool CClientVehicleManager::HasTurret ( unsigned long ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_TURRENT ) );
}


bool CClientVehicleManager::HasSirens ( unsigned long ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_SIRENS ) );
}


bool CClientVehicleManager::HasLandingGears ( unsigned long ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_LANDING_GEARS ) );
}


bool CClientVehicleManager::HasAdjustableProperty ( unsigned long ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_ADJUSTABLE_PROPERTY ) );
}


bool CClientVehicleManager::HasSmokeTrail ( unsigned long ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_SMOKE_TRAIL ) );
}


bool CClientVehicleManager::HasDamageModel ( unsigned long ulModel )
{
    return HasDamageModel ( GetVehicleType ( ulModel ) );
}


bool CClientVehicleManager::HasDamageModel ( eClientVehicleType Type )
{
    return ( Type == CLIENTVEHICLE_TRAILER ||
             Type == CLIENTVEHICLE_MONSTERTRUCK ||
             Type == CLIENTVEHICLE_QUADBIKE ||
             Type == CLIENTVEHICLE_HELI ||
             Type == CLIENTVEHICLE_PLANE ||
             Type == CLIENTVEHICLE_CAR );
}


list < CClientVehicle* > ::iterator CClientVehicleManager::IterGet ( CClientVehicle* pVehicle )
{
    // Find it in our list
    list < CClientVehicle* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pVehicle )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_List.begin ();
}


list < CClientVehicle* > ::reverse_iterator CClientVehicleManager::IterGetReverse ( CClientVehicle* pVehicle )
{
    // Find it in our list
    list < CClientVehicle* > ::reverse_iterator iter = m_List.rbegin ();
    for ( ; iter != m_List.rend (); iter++ )
    {
        if ( *iter == pVehicle )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_List.rbegin ();
}


void CClientVehicleManager::RemoveFromList ( CClientVehicle* pVehicle )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pVehicle );
    }
}


bool CClientVehicleManager::Exists ( CClientVehicle* pVehicle )
{
    list < CClientVehicle* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( *iter == pVehicle )
        {
            return true;
        }
    }

    return false;
}


bool CClientVehicleManager::IsVehicleLimitReached ( void )
{
    // GTA allows max 110 vehicles. We restrict ourselves to 64 for now
    // due to FPS issues and crashes around 100 vehicles.
    return g_pGame->GetPools ()->GetVehicleCount () >= 64;
}


void CClientVehicleManager::OnCreation ( CClientVehicle * pVehicle )
{
    m_StreamedIn.push_back ( pVehicle );
}


void CClientVehicleManager::OnDestruction ( CClientVehicle * pVehicle )
{
    m_StreamedIn.remove ( pVehicle );
}
