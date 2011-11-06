/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleManager.cpp
*  PURPOSE:     Vehicle entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static unsigned char g_ucMaxPassengers [] = { 3, 1, 1, 1, 3, 3, 0, 1, 1, 3, 1, 1, 1, 3, 1, 1,       // 400->415
                                       3, 1, 3, 1, 3, 3, 1, 1, 1, 0, 3, 3, 3, 1, 0, 8,              // 416->431
                                       0, 1, 1, 255, 1, 8, 3, 1, 3, 0, 1, 1, 1, 3, 0, 1,            // 432->447
                                       0, 1, 255, 1, 0, 0, 0, 1, 1, 1, 3, 3, 1, 1, 1,               // 448->462
                                       1, 1, 1, 3, 3, 1, 1, 3, 1, 0, 0, 1, 1, 0, 1, 1,              // 463->478
                                       3, 1, 0, 3, 1, 0, 0, 0, 3, 1, 1, 3, 1, 3, 0, 1,              // 479->494
                                       1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 0, 0,              // 495->510
                                       1, 0, 0, 1, 1, 3, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,              // 511->526
                                       1, 1, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 3, 1,                 // 527->541
                                       1, 1, 1, 1, 3, 3, 1, 1, 3, 3, 1, 0, 1, 1, 1, 1,              // 542->557
                                       1, 1, 3, 3, 1, 1, 0, 1, 3, 3, 0, 255, 1, 0, 0,               // 558->572
                                       1, 0, 1, 1, 1, 1, 3, 3, 1, 3, 0, 255, 3, 1, 1, 1,            // 573->588
                                       1, 255, 255, 1, 1, 1, 0, 3, 3, 3, 1, 1, 1, 1, 1,             // 589->604
                                       3, 1, 255, 255, 255, 3, 255, 255 };                          // 605->611

// List over all vehicles with their special attributes
#define VEHICLE_HAS_TURRENT             0x001UL //1
#define VEHICLE_HAS_SIRENS              0x002UL //2
#define VEHICLE_HAS_LANDING_GEARS       0x004UL //4
#define VEHICLE_HAS_ADJUSTABLE_PROPERTY 0x008UL //8
#define VEHICLE_HAS_SMOKE_TRAIL         0x010UL //16
#define VEHICLE_HAS_TAXI_LIGHTS         0x020UL //32
#define VEHICLE_HAS_SEARCH_LIGHT        0x040UL //64

unsigned long g_ulVehicleAttributes [] = {
  0, 0, 0, 0, 0, 0, 8, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 32, 0, 0, 2, 0,    // 400-424
  0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,    // 425-449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 450-474
  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 64, 0, 0,    // 475-499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 4, 12, 0, 0, 2, 8, // 500-524
  8, 0, 0, 2, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,    // 525-549
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 550-574
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 2, 2, 2, 2,   // 575-599
  0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


static eVehicleType gs_vehicleTypes [] = {
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BOAT,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_CAR,
    VEHICLE_BOAT, VEHICLE_HELI, VEHICLE_BIKE, VEHICLE_TRAIN, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_BOAT,
    VEHICLE_BOAT, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE,
    VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_PLANE, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_BIKE, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_QUADBIKE, VEHICLE_BOAT, VEHICLE_BOAT, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BMX, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BMX, VEHICLE_BMX, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_PLANE,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_BIKE,
    VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_TRAIN,
    VEHICLE_TRAIN, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_MONSTERTRUCK, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAIN, VEHICLE_TRAIN, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_TRAIN,
    VEHICLE_TRAILER, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAILER, VEHICLE_TRAILER, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_TRAILER, VEHICLE_TRAILER
};

CVehicleManager::CVehicleManager ( void )
{
    assert ( NUMELMS ( g_ucMaxPassengers ) == 212 );
    assert ( NUMELMS ( g_ulVehicleAttributes ) == 212 );
    assert ( NUMELMS ( gs_vehicleTypes ) == 212 );

    // Init
    m_bDontRemoveFromList = false;
}


CVehicleManager::~CVehicleManager ( void )
{
    DeleteAll ();
}


CVehicle* CVehicleManager::Create ( unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2, CElement* pParent, CXMLNode* pNode )
{
    // Create the vehicle
    CVehicle* pVehicle = new CVehicle ( this, pParent, pNode, usModel, ucVariant, ucVariant2 );

    // Invalid vehicle id?
    if ( pVehicle->GetID () == INVALID_ELEMENT_ID )
    {
        delete pVehicle;
        return NULL;
    }

    // Return the created vehicle
    return pVehicle;
}


CVehicle* CVehicleManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the vehicle
    srand ( GetTickCount32() );
    CVehicle* pVehicle = new CVehicle ( this, pParent, &Node, 400, rand() % 5, rand() % 5 );

    // Verify the vehicle id and load the data from xml
    if ( pVehicle->GetID () == INVALID_ELEMENT_ID ||
         !pVehicle->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pVehicle;
        return NULL;
    }

    // Return the created vehicle
    return pVehicle;
}


void CVehicleManager::DeleteAll ( void )
{
    // Delete all items
    m_bDontRemoveFromList = true;
    list < CVehicle* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
    m_bDontRemoveFromList = false;
}


void CVehicleManager::RemoveFromList ( CVehicle* pVehicle )
{
    if ( !m_bDontRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pVehicle );
    }
}


bool CVehicleManager::Exists ( CVehicle* pVehicle )
{
    // Try to find the vehicle ID in the list
    list < CVehicle* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( (*iter) == pVehicle )
        {
            return true;
        }
    }

    // Couldn't find it
    return false;
}


bool CVehicleManager::IsValidModel ( unsigned int ulVehicleModel )
{
    return ulVehicleModel >= 400 && ulVehicleModel <= 611 && ulVehicleModel != 570;
}

eVehicleType CVehicleManager::GetVehicleType ( unsigned short usModel )
{
    if ( !IsValidModel ( usModel ) )
        return VEHICLE_NONE;

    return gs_vehicleTypes [ usModel - 400 ];
}


bool CVehicleManager::IsValidUpgrade ( unsigned short usUpgrade )
{
    return ( usUpgrade >= 1000 && usUpgrade <= 1193 );
}


unsigned int CVehicleManager::GetMaxPassengers ( unsigned int uiVehicleModel )
{
    if ( IsValidModel ( uiVehicleModel ) )
    {
        return g_ucMaxPassengers [uiVehicleModel - 400];
    }

    return 0xFF;
}


bool CVehicleManager::HasTurret ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_TURRENT ) );
}


bool CVehicleManager::HasSirens ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_SIRENS ) );
}

bool CVehicleManager::HasTaxiLight ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_TAXI_LIGHTS ) );
}

bool CVehicleManager::HasLandingGears ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_LANDING_GEARS ) );
}


bool CVehicleManager::HasAdjustableProperty ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_ADJUSTABLE_PROPERTY ) );
}


bool CVehicleManager::HasSmokeTrail ( unsigned int uiModel )
{
    return ( IsValidModel ( uiModel ) &&
             ( g_ulVehicleAttributes[ uiModel - 400 ] & VEHICLE_HAS_SMOKE_TRAIL ) );
}

bool CVehicleManager::IsTrailer ( unsigned int uiVehicleModel )
{
    return ( IsValidModel ( uiVehicleModel ) &&
             ( gs_vehicleTypes [uiVehicleModel - 400] == VEHICLE_TRAILER ) );
}

bool CVehicleManager::HasDamageModel ( unsigned short usModel )
{
    return HasDamageModel ( GetVehicleType ( usModel ) );
}


bool CVehicleManager::HasDamageModel ( eVehicleType Type )
{
    switch ( Type )
    {
        case VEHICLE_TRAILER:
        case VEHICLE_MONSTERTRUCK:
        case VEHICLE_QUADBIKE:
        case VEHICLE_HELI:
        case VEHICLE_PLANE:
        case VEHICLE_CAR:
            return true;
        default:
            return false;
    }
}

bool CVehicleManager::HasDoors ( unsigned short usModel )
{
    bool bHasDoors = false;

    if ( HasDamageModel ( usModel ) == true )
    {
        switch ( usModel )
        {
            case VT_BFINJECT:
            case VT_RCBANDIT:
            case VT_CADDY:
            case VT_RCRAIDER:
            case VT_BAGGAGE:
            case VT_DOZER:
            case VT_FORKLIFT:
            case VT_TRACTOR:
            case VT_RCTIGER:
            case VT_BANDITO:
            case VT_KART:
            case VT_MOWER:
            case VT_RCCAM:
            case VT_RCGOBLIN:
                break;
            default:
                bHasDoors = true;
        }
    }

    return bHasDoors;
}

CVehicleColor CVehicleManager::GetRandomColor ( unsigned short usModel )
{
    return m_ColorManager.GetRandomColor ( usModel );
}

void CVehicleManager::GetVehiclesOfType ( unsigned int uiModel, lua_State* luaVM )
{
    assert ( luaVM );

    // Add all the matching vehicles to the table
    unsigned int uiIndex = 0;
    list < CVehicle* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        // Add it to the table
        lua_pushnumber ( luaVM, ++uiIndex );
        lua_pushelement ( luaVM, *iter );
        lua_settable ( luaVM, -3 );
    }
}
