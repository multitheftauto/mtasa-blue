/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPed.cpp
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

char szBodyPartNameEmpty [] = "";
struct SBodyPartName { const char szName [32]; };
SBodyPartName BodyPartNames [10] =
{ {"Unknown"}, {"Unknown"}, {"Unknown"}, {"Torso"}, {"Ass"},
{"Left Arm"}, {"Right Arm"}, {"Left Leg"}, {"Right Leg"}, {"Head"} };

CPed::CPed ( CPedManager* pPedManager, CElement* pParent, CXMLNode* pNode, unsigned short usModel ) : CElement ( pParent, pNode )
{
    // Init
    m_pPedManager = pPedManager;

    m_iType = CElement::PED;
    SetTypeName ( "ped" );

    m_usModel = usModel;
    m_bDucked = false;
    m_bIsChoking = false;
    m_bWearingGoggles = false;

    m_fHealth = 0.0f;
    m_ulHealthChangeTime = 0;
    m_fArmor = 0.0f;
    m_ulArmorChangeTime = 0;
    
    memset ( &m_fStats[0], 0, sizeof ( m_fStats ) );
    m_fStats [ 24 ] = 569.0f;           // default max_health

    m_pClothes = new CPlayerClothes;
    m_pClothes->DefaultClothes ();
    m_bHasJetPack = false;
    m_bHeadless = false;
    m_bInWater = false;
    m_bOnGround = true;
    m_bIsPlayer = false;
    m_bFrozen = false;
    m_bIsOnFire = false;

    m_pTasks = new CPlayerTasks;

    m_ucWeaponSlot = 0;
    memset ( &m_Weapons[0], 0, sizeof ( m_Weapons ) );
    m_ucAlpha = 255;
    m_pContactElement = NULL;
    m_bIsDead = true;
    m_ulLastDieTime = 0;
    m_bSpawned = false;
    m_fRotation = 0.0f;
    m_pTargetedEntity = NULL;
    m_ucFightingStyle = 15; // STYLE_GRAB_KICK
    m_iMoveAnim = MOVE_DEFAULT;
    m_fGravity = 0.008f;
    m_bDoingGangDriveby = false;
    m_bStealthAiming = false;

    m_pVehicle = NULL;
    m_uiVehicleSeat = INVALID_VEHICLE_SEAT;
    m_uiVehicleAction = CPed::VEHICLEACTION_NONE;
    m_ulVehicleActionStartTime = 0;

    m_vecVelocity.fX = m_vecVelocity.fY = m_vecVelocity.fZ = 0.0f;

    m_pSyncer = NULL;

    m_bCollisionsEnabled = true;

    // Add us to the Ped manager
    if ( pPedManager )
    {
        pPedManager->AddToList ( this );
    }
}


CPed::~CPed ( void )
{
    // Make sure we've no longer occupied any vehicle
    if ( m_pVehicle )
    {
        m_pVehicle->SetOccupant ( NULL, m_uiVehicleSeat );
    }

    SetSyncer ( NULL );

    delete m_pClothes;
    delete m_pTasks;

    if ( m_pContactElement )
        m_pContactElement->RemoveOriginSourceUser ( this );

    // Remove us from the Ped manager
    Unlink ();
}


void CPed::Unlink ( void )
{
    // Remove us from the Ped manager
    if ( m_pPedManager )
    {
        m_pPedManager->RemoveFromList ( this );
    }
}


void CPed::GetMatrix( CMatrix& matrix )
{
    CVector vecRotation;
    vecRotation.fZ = GetRotation();
    // Turn inverted rotation into non-inverted
    matrix.SetRotation( -vecRotation );
    matrix.vPos = GetPosition();
}


void CPed::SetMatrix( const CMatrix& matrix )
{
    SetPosition( matrix.vPos );
    CVector vecRotation = matrix.GetRotation();
    // Turn non-inverted rotation into inverted
    SetRotation( -vecRotation.fZ );
}


bool CPed::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <ped> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <ped> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <ped> (line %u)\n", m_uiLine );
        return false;
    }

    float fRotation = 0.0f;
    GetCustomDataFloat ( "rotZ", fRotation, true );
    m_fRotation = ConvertDegreesToRadians ( fRotation );

    // Grab the "model" data
    int iTemp;
    if ( GetCustomDataInt ( "model", iTemp, true ) )
    {
        // Is it valid?
        unsigned short usModel = static_cast < unsigned short > ( iTemp );
        if ( CPedManager::IsValidModel ( usModel ) )
        {
            // Remember it and generate a new random color
            m_usModel = usModel;
        }
        else
        {
            CLogger::ErrorPrintf ( "Bad 'model' id specified in <ped> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf ( "Bad/missing 'model' attribute in <ped> (line %u)\n", m_uiLine );
        return false;
    }

    GetCustomDataFloat ( "health", m_fHealth, true );
    GetCustomDataFloat ( "armor", m_fArmor, true );

    if ( GetCustomDataInt ( "interior", iTemp, true ) )
        m_ucInterior = static_cast < unsigned char > ( iTemp );

    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    if ( !GetCustomDataBool ( "collisions", m_bCollisionsEnabled, true ) )
        m_bCollisionsEnabled = true;

    if ( GetCustomDataInt ( "alpha", iTemp, true ) )
        m_ucAlpha = static_cast < unsigned char > ( iTemp );

    bool bFrozen;
    if ( GetCustomDataBool ( "frozen", bFrozen, true ) )
        m_bFrozen = bFrozen;

    return true;
}


bool CPed::HasValidModel ( void )
{
    return CPedManager::IsValidModel ( m_usModel );
}

void CPed::SetWeaponSlot ( unsigned char ucSlot )
{
    if ( ucSlot < WEAPON_SLOTS )
    {
        m_ucWeaponSlot = ucSlot;
    }
}


CWeapon* CPed::GetWeapon ( unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        return &m_Weapons [ ucSlot ];
    }
    return NULL;
}



unsigned char CPed::GetWeaponType ( unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        return m_Weapons [ ucSlot ].ucType;
    }
    return 0;
}


void CPed::SetWeaponType ( unsigned char ucType, unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        m_Weapons [ ucSlot ].ucType = ucType;
    }
}


unsigned short CPed::GetWeaponAmmoInClip ( unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        return m_Weapons [ ucSlot ].usAmmoInClip;
    }
    return 0;
}


void CPed::SetWeaponAmmoInClip ( unsigned short usAmmoInClip, unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        m_Weapons [ ucSlot ].usAmmoInClip = usAmmoInClip;
    }
}


unsigned short CPed::GetWeaponTotalAmmo ( unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        return m_Weapons [ ucSlot ].usAmmo;
    }
    return 0;
}


void CPed::SetWeaponTotalAmmo ( unsigned short usTotalAmmo, unsigned char ucSlot )
{
    if ( ucSlot == 0xFF )
        ucSlot = m_ucWeaponSlot;
    if ( ucSlot < WEAPON_SLOTS )
    {
        m_Weapons [ ucSlot ].usAmmo = usTotalAmmo;
    }
}

float CPed::GetMaxHealth ( void )
{
    // TODO: Verify this formula

    // Grab his player health stat
    float fStat = GetPlayerStat ( 24 /*MAX_HEALTH*/ );

    // Do a linear interpolation to get how much health this would allow
    // Assumes: 100 health = 569 stat, 200 health = 1000 stat.
    float fMaxHealth = 100.0f + ( 100.0f / 431.0f * ( fStat - 569.0f ) );

    // Return the max health. Make sure it can't be below 1
    if ( fMaxHealth < 1.0f ) fMaxHealth = 1.0f;
    return fMaxHealth;
}


const char* CPed::GetBodyPartName ( unsigned char ucID )
{
    if ( ucID <= NUMELMS( BodyPartNames ) )
    {
        return BodyPartNames [ucID].szName;
    }

    return szBodyPartNameEmpty;
}


void CPed::SetContactElement ( CElement* pElement )
{
    if ( pElement != m_pContactElement )
    {
        if ( m_pContactElement )
            m_pContactElement->RemoveOriginSourceUser ( this );

        if ( pElement )
            pElement->AddOriginSourceUser ( this );

        m_pContactElement = pElement;
    }
}


void CPed::SetIsDead ( bool bDead )
{
    if ( !m_bIsDead && bDead )
    {
        m_ulLastDieTime = GetTime ();
    }

    m_bIsDead = bDead;
}


CVehicle* CPed::SetOccupiedVehicle ( CVehicle* pVehicle, unsigned int uiSeat )
{
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Store it
        m_pVehicle = pVehicle;
        m_uiVehicleSeat = uiSeat;

        // Make sure the vehicle knows
        if ( m_pVehicle )
        {
            bAlreadyIn = true;
            pVehicle->SetOccupant ( this, uiSeat );
            bAlreadyIn = false;
        }
    }

    return m_pVehicle;
}


void CPed::SetVehicleAction ( unsigned int uiAction )
{
    m_uiVehicleAction = uiAction;
    m_ulVehicleActionStartTime = GetTime ();
}


bool CPed::IsAttachToable ( void )
{
    // We're not attachable if we're inside a vehicle (that would get messy)
    if ( !GetOccupiedVehicle () )
    {
        return true;
    }
    return false;
}

void CPed::SetSyncer ( CPlayer* pPlayer )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the last Player if any
        bAlreadyIn = true;
        if ( m_pSyncer )
        {
            m_pSyncer->RemoveSyncingPed ( this );
        }

        // Update the vehicle
        if ( pPlayer )
        {
            pPlayer->AddSyncingPed ( this );
        }
        bAlreadyIn = false;

        // Set it
        m_pSyncer = pPlayer;
    }
}
