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

unsigned char g_ucVariants [212];

CVehicleManager::CVehicleManager ( void )
{
    assert ( NUMELMS ( g_ucMaxPassengers ) == 212 );
    assert ( NUMELMS ( g_ulVehicleAttributes ) == 212 );
    assert ( NUMELMS ( gs_vehicleTypes ) == 212 );

    // Init
    m_bDontRemoveFromList = false;

    int iVehicleID = 0;
    for ( int i = 0; i <= 212; i++ )
    {
        g_ucVariants[i] = 255;
        iVehicleID = i + 400;
        switch ( iVehicleID )
        {
            case 416:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 435:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 450:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 607:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 485:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 433:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 499:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 581:
            {
                g_ucVariants[i] = 4;
                break;
            }
            case 424:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 504:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 422:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 482:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 457:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 483:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 415:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 437:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 472:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 521:
            {
                g_ucVariants[i] = 4;
                break;
            }
            case 407:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 455:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 434:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 502:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 503:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 571:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 595:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 484:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 500:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 556:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 557:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 423:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 414:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 522:
            {
                g_ucVariants[i] = 4;
                break;
            }
            case 470:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 404:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 600:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 413:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 453:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 442:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 440:
            {
                g_ucVariants[i] = 5;
                break;
            }
            case 543:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 605:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 428:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 535:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 439:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 506:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 601:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 459:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 449:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 408:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 583:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 552:
            {
                g_ucVariants[i] = 1;
                break;
            }
            case 478:
            {
                g_ucVariants[i] = 2;
                break;
            }
            case 555:
            {
                g_ucVariants[i] = 0;
                break;
            }
            case 456:
            {
                g_ucVariants[i] = 3;
                break;
            }
            case 477:
            {
                g_ucVariants[i] = 0;
                break;
            }
        }
    }
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
    RandomizeRandomSeed ();

    CVehicle* pVehicle = new CVehicle ( this, pParent, &Node, 400, 254, 254 );

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

void CVehicleManager::GetRandomVariation ( unsigned short usModel, unsigned char &ucVariant, unsigned char &ucVariant2 )
{
    RandomizeRandomSeed ();
    ucVariant = 255;
    ucVariant2 = 255;
    // Valid model?
    if ( IsValidModel( usModel ) && g_ucVariants[ usModel - 400 ] != 255 )
    {
        if ( usModel == 581 || usModel == 457 || usModel == 512 || usModel == 522 )
        {
            unsigned char ucVariants = g_ucVariants [usModel - 400];
            unsigned char ucVariantsHalf = ucVariants / 2;
            // e.g. 581 ( BF400 )
            // first 3 properties are Exhaust
            // last 2 are fairings.


            ucVariant = ( rand ( ) % 4 ) - 1;

            ucVariant2 = ( rand ( ) % 3 );
            ucVariant2 += 3;
            return;
        }
        else if ( usModel == 535 )
        {
            // Slamvan has steering wheel "extras" we want one of those so default cannot be an option.
            ucVariant = rand ( ) % g_ucVariants [usModel - 400];
            return;
        }
        // e.g. ( rand () % ( 5 + 1 ) ) - 1
        // Can generate 6 then minus 1 = 5
        // Can generate 0 then minus 1 = -1 (255) (default model with nothing)
        ucVariant = ( rand ( ) % (g_ucVariants [usModel - 400] + 1) );
    }
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
