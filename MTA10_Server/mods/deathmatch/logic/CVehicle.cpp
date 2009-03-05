/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicle.cpp
*  PURPOSE:     Vehicle entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

CVehicle::CVehicle ( CVehicleManager* pVehicleManager, CElement* pParent, CXMLNode* pNode, unsigned short usModel ) : CElement ( pParent, pNode )
{
    // Init
    m_pVehicleManager = pVehicleManager;
    m_usModel = usModel;
	m_pUpgrades = new CVehicleUpgrades ( this );

    m_iType = CElement::VEHICLE;
    SetTypeName ( "vehicle" );
    m_fHealth = DEFAULT_VEHICLE_HEALTH;
    m_ulHealthChangeTime = 0;
    m_ulBlowTime = 0;
    m_ulIdleTime = GetTickCount ();
    m_fTurretPositionX = 0;
    m_fTurretPositionY = 0;
    m_bSirenActive = false;
    m_bLandingGearDown = true;
    m_usAdjustableProperty = 0;
    m_bIsFrozen = false;
    m_pSyncer = NULL;
    GetInitialDoorStates ( m_ucDoorStates );
    memset ( m_ucWheelStates, 0, sizeof ( m_ucWheelStates ) );
    memset ( m_ucPanelStates, 0, sizeof ( m_ucPanelStates ) );
    memset ( m_ucLightStates, 0, sizeof ( m_ucLightStates ) );
    m_ucOverrideLights = 0;
    m_pTowedVehicle = NULL;
    m_pTowedByVehicle = NULL;
    m_ucPaintjob = 0xFF;
	m_ucMaxPassengersOverride = VEHICLE_PASSENGERS_UNDEFINED;

    m_bRespawnInfoChanged = false;
    m_fRespawnHealth = DEFAULT_VEHICLE_HEALTH;
    m_bRespawnEnabled = false;
    m_ulRespawnTime = 10000;
    m_ulIdleRespawnTime = 60000;

    m_bEngineOn = false;
    m_bLocked = false;
    m_bDoorsUndamageable = false;
    m_bDamageProof = false;
    m_bGunsEnabled = false;
    m_bFuelTankExplodable = false;
    m_bOnGround = true;
    m_bSmokeTrail = false;
    m_ucAlpha = 255;
    m_pJackingPlayer = NULL;
    m_bInWater = false;
    m_bDerailed = false;
    m_bIsDerailable = true;
    m_bTrainDirection = true;

    // Initialize the occupied Players
    for ( int i = 0; i < MAX_VEHICLE_SEATS; i++ )
    {
        m_pOccupants [i] = NULL;
    }

    // Add us to the vehicle manager
    pVehicleManager->AddToList ( this );

    // Randomize our color
    RandomizeColor ();

    // Generate a random reg plate
    GenerateRegPlate ();
}


CVehicle::~CVehicle ( void )
{
    if ( m_pJackingPlayer && m_pJackingPlayer->GetJackingVehicle () == this )
    {
        if ( m_pJackingPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_JACKING )
        {
            m_pJackingPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
        }
        m_pJackingPlayer->SetJackingVehicle ( NULL );
    }

    // Unset any tow links
    if ( m_pTowedVehicle )
        m_pTowedVehicle->SetTowedByVehicle ( NULL );

    if ( m_pTowedByVehicle )
        m_pTowedByVehicle->SetTowedVehicle ( NULL );

    // Make sure nobody's syncing us
    SetSyncer ( NULL );

    // Unreference from our occupators
    for ( unsigned int i = 0; i < MAX_VEHICLE_SEATS; i++ )
    {
        if ( m_pOccupants [i] )
        {
            m_pOccupants [i]->SetOccupiedVehicle ( NULL, 0 );
            m_pOccupants [i]->SetVehicleAction ( CPed::VEHICLEACTION_NONE );
        }
    }
	delete m_pUpgrades;

    // Remove us from the vehicle manager
    Unlink ();
}


void CVehicle::Unlink ( void )
{
	// Remove us from the vehicle manager
	m_pVehicleManager->RemoveFromList ( this );
}


bool CVehicle::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <vehicle> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <vehicle> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <vehicle> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "rotX", "rotY" and "rotZ" data
    GetCustomDataFloat ( "rotX", m_vecRotationDegrees.fX, true );
    GetCustomDataFloat ( "rotY", m_vecRotationDegrees.fY, true );
    GetCustomDataFloat ( "rotZ", m_vecRotationDegrees.fZ, true );

    // Wrap them around 360
    m_vecRotationDegrees.fX = WrapAround ( m_vecRotationDegrees.fX, 360.0f );
    m_vecRotationDegrees.fY = WrapAround ( m_vecRotationDegrees.fY, 360.0f );
    m_vecRotationDegrees.fZ = WrapAround ( m_vecRotationDegrees.fZ, 360.0f );

    // Set the respawn matrix
    SetRespawnPosition ( m_vecPosition );
    SetRespawnRotationDegrees ( m_vecRotationDegrees );

    // Grab the "model" data
    int iTemp;
    if ( GetCustomDataInt ( "model", iTemp, true ) )
    {
        // Is it valid?
        if ( CVehicleManager::IsValidModel ( iTemp ) )
        {
            // Remember it and generate a new random color
            m_usModel = static_cast < unsigned short > ( iTemp );
            m_Color = RandomizeColor ();
            GetInitialDoorStates ( m_ucDoorStates );

            m_usAdjustableProperty = 0;
        }
        else
        {
            CLogger::ErrorPrintf ( "Bad 'model'(%d) id specified in <vehicle> (line %u)\n", iTemp, m_uiLine );
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf ( "Bad/missing 'model' attribute in <vehicle> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "turretX" data
    if ( GetCustomDataFloat ( "turretX", m_fTurretPositionX, true ) )
    {
        m_fTurretPositionX = ConvertDegreesToRadians ( m_fTurretPositionX );
    }

    // Grab the "turretY" data
    if ( GetCustomDataFloat ( "turretY", m_fTurretPositionY, true ) )
    {
        m_fTurretPositionY = ConvertDegreesToRadians ( m_fTurretPositionY );
    }

    // Grab the "health" data
    if ( GetCustomDataFloat ( "health", m_fHealth, true ) )
    {
        if ( m_fHealth < 0.0f )
            m_fHealth = 0.0f;
    }

    // Grab the "Sirens" data
    if ( !GetCustomDataBool ( "sirens", m_bSirenActive, true ) )
    {
        m_bSirenActive = false;
    }

    // Grab the "landingGearDown" data
    if ( !GetCustomDataBool ( "landingGearDown", m_bLandingGearDown, true ) )
    {
        m_bLandingGearDown = true;
    }

    if ( !GetCustomDataBool ( "locked", m_bLocked, true ) )
    {
        m_bLocked = false;
    }

    // Grab the "specialState" data
    if ( GetCustomDataInt ( "specialState", iTemp, true ) )
    {
        m_usAdjustableProperty = static_cast < unsigned short > ( iTemp );
    }
    else
    {
        m_usAdjustableProperty = 0;
    }

    // Grab the "color" data
    char szTemp [ 256 ];
    if ( GetCustomDataString ( "color", szTemp, 256, true ) )
    {
        char* sz1 = strtok ( szTemp, "," );
        char* sz2 = strtok ( NULL, "," );
        char* sz3 = strtok ( NULL, "," );
        char* sz4 = strtok ( NULL, "," );
        if ( sz1 )
            m_Color.SetColor1 ( atoi ( sz1 ) );
        if ( sz2 )
            m_Color.SetColor2 ( atoi ( sz2 ) );
        if ( sz3 )
            m_Color.SetColor3 ( atoi ( sz3 ) );
        if ( sz4 )
            m_Color.SetColor4 ( atoi ( sz4 ) );
    }            

    if ( GetCustomDataInt ( "paintjob", iTemp, true ) )
        m_ucPaintjob = static_cast < unsigned char > ( iTemp );

    if ( GetCustomDataString ( "upgrades", szTemp, 256, true ) )
    {
        if ( m_pUpgrades )
        {
            if ( strcmp ( szTemp, "all" ) == 0 )
            {
                m_pUpgrades->AddAllUpgrades ();
            }
            else
            {
                bool bTemp = true;
                while ( char* token = strtok ( ( bTemp ) ? szTemp : NULL, " " ) )
                {
                    bTemp = false;
                    unsigned short usUpgrade = static_cast < unsigned short > ( atoi ( token ) );
                    if ( CVehicleUpgrades::IsValidUpgrade ( usUpgrade ) )
                    {
                        m_pUpgrades->AddUpgrade ( usUpgrade );
                    }
                }
            }
        }
    }

    if ( GetCustomDataString ( "plate", szTemp, 9, true ) )
    {
        SetRegPlate ( szTemp );
    }

	if ( GetCustomDataInt ( "interior", iTemp, true ) )
        m_ucInterior = static_cast < unsigned char > ( iTemp );

	if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    return true;
}


const CVector & CVehicle::GetPosition ( void )
{
    // Are we attached to something?
    if ( m_pAttachedTo )
    {
        // Update our stored position to where we should be, before returning
        m_vecPosition = m_pAttachedTo->GetPosition () + m_vecAttachedPosition;
    }
    return m_vecPosition;
}


void CVehicle::SetPosition ( const CVector & vecPosition )
{
    // If we're attached to something, dont allow any change
    if ( m_pAttachedTo ) return;

    // Different to where we are now?
    if ( m_vecPosition != vecPosition )
    {
        // Update our stored vectors
        m_vecLastPosition = m_vecPosition;
        m_vecPosition = vecPosition;
    }
}


void CVehicle::SetModel ( unsigned short usModel )
{
    if ( usModel != m_usModel )
    {
        m_usModel = usModel;
        RandomizeColor ();
        GetInitialDoorStates ( m_ucDoorStates );
    }
}


bool CVehicle::HasValidModel ( void )
{
    return CVehicleManager::IsValidModel ( m_usModel );
}


CVehicleColor& CVehicle::RandomizeColor ( void )
{
    // Grab a random color for this vehicle and return it 
    m_Color = m_pVehicleManager->GetRandomColor ( m_usModel );
    return m_Color;
}


void CVehicle::GetTurretPosition ( float& fPositionX, float& fPositionY )
{
    fPositionX = m_fTurretPositionX;
    fPositionY = m_fTurretPositionY;
}


void CVehicle::SetTurretPosition ( float fPositionX, float fPositionY )
{
    m_fTurretPositionX = fPositionX;
    m_fTurretPositionY = fPositionY;
}


CPed* CVehicle::GetOccupant ( unsigned int uiSeat )
{    
    if ( uiSeat < MAX_VEHICLE_SEATS )
    {
        return m_pOccupants [uiSeat];
    }

    return NULL;
}


CPed* CVehicle::GetFirstOccupant ( void )
{
    // Try finding a seat with a Player in it
    unsigned int i = 0;
    for ( ; i < MAX_VEHICLE_SEATS; i++ )
    {
        if ( m_pOccupants [i] )
        {
            return m_pOccupants [i];
        }
    }

    // No seats with Players in it
    return NULL;
}


CPed* CVehicle::GetController ( void )
{
    CPed* pController = m_pOccupants [ 0 ];

    if ( !pController )
    {
        CVehicle* pCurrentVehicle = this;
        CVehicle* pTowedByVehicle = m_pTowedByVehicle;
        pController = pCurrentVehicle->GetOccupant ( 0 );
        while ( pTowedByVehicle )
        {
            pCurrentVehicle = pTowedByVehicle;
            pTowedByVehicle = pCurrentVehicle->GetTowedByVehicle ();
            CPed* pCurrentDriver = pCurrentVehicle->GetOccupant ( 0 );
            if ( pCurrentDriver )
                pController = pCurrentDriver;
        }
    }

    return pController;
}


bool CVehicle::SetOccupant ( CPed* pPed, unsigned int uiSeat )
{
    // CPlayer::SetOccupiedVehicle will also call this so we need to prevent an infinite recursive loop
    static bool bAlreadySetting = false;
    if ( !bAlreadySetting )
    {
        // Set the Player
        m_pOccupants [uiSeat] = pPed;

        // Make sure the Player record is up to date
        if ( pPed )
        {
            bAlreadySetting = true;
            pPed->SetOccupiedVehicle ( this, uiSeat );
            bAlreadySetting = false;
        }

        return true;
    }

    return true;
}


void CVehicle::SetSyncer ( CPlayer* pPlayer )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the last Player if any
        bAlreadyIn = true;
        if ( m_pSyncer )
        {
            m_pSyncer->RemoveSyncingVehicle ( this );
        }

        // Update the vehicle
        if ( pPlayer )
        {
            pPlayer->AddSyncingVehicle ( this );
        }
        bAlreadyIn = false;

        // Set it
        m_pSyncer = pPlayer;
    }
}


unsigned char CVehicle::GetMaxPassengers ( void )
{
	return ( ( m_ucMaxPassengersOverride == VEHICLE_PASSENGERS_UNDEFINED ) ? CVehicleManager::GetMaxPassengers ( m_usModel ) : m_ucMaxPassengersOverride );
}


unsigned char CVehicle::GetFreePassengerSeat ( void )
{
    // Grab the max passengers this vehicle can have and check the rage
    unsigned char ucMaxPassengers = GetMaxPassengers ();
    if ( ucMaxPassengers < MAX_VEHICLE_SEATS )
    {
        // Check for free slots
        for ( int i = 1; i < ucMaxPassengers + 1; i++ )
        {
            if ( !m_pOccupants [i] )
            {
                return static_cast < unsigned char > ( i );
            }
        }
    }

    return 0xFF;
}


void CVehicle::SetUpgrades ( CVehicleUpgrades* pUpgrades )
{
    delete m_pUpgrades;

    m_pUpgrades = new CVehicleUpgrades ( this, pUpgrades );
}


bool CVehicle::SetTowedVehicle ( CVehicle* pVehicle )
{
    if ( m_pTowedVehicle && pVehicle )
    {
        m_pTowedVehicle->m_pTowedByVehicle = NULL;
        m_pTowedVehicle = NULL;
    }

    if ( pVehicle )
    {
        // Are we trying to establish a circular loop? (would freeze everything up)
        CVehicle* pCircTestVehicle = pVehicle;
        while ( pCircTestVehicle )
        {
            if ( pCircTestVehicle == this )
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedVehicle;
        }

        pVehicle->m_pTowedByVehicle = this;
    }

    m_pTowedVehicle = pVehicle;
    return true;
}


bool CVehicle::SetTowedByVehicle ( CVehicle* pVehicle )
{
    if ( m_pTowedByVehicle && pVehicle )
    {
        m_pTowedByVehicle->m_pTowedVehicle = NULL;
        m_pTowedByVehicle = NULL;
    }

    if ( pVehicle )
    {
        // Are we trying to establish a circular loop? (would freeze everything up)
        CVehicle* pCircTestVehicle = pVehicle;
        while ( pCircTestVehicle )
        {
            if ( pCircTestVehicle == this )
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedByVehicle;
        }

        pVehicle->m_pTowedVehicle = this;
    }

    m_pTowedByVehicle = pVehicle;
    return true;
}


void CVehicle::PutAtRespawnLocation ( void )
{
    m_vecLastPosition = m_vecPosition;
    m_vecPosition = m_vecRespawnPosition;
    m_vecRotationDegrees = m_vecRespawnRotationDegrees;
    m_fHealth = m_fRespawnHealth;
}


void CVehicle::SetRegPlate ( const char* szRegPlate )
{
    // Copy the text and make sure non-used chars are nulled.
    memset ( m_szRegPlate, 0, 9 );
    strncpy ( m_szRegPlate, szRegPlate, 9 );
}


void CVehicle::GenerateRegPlate ( void )
{
    // For all our 8 letters
    for ( int i = 0; i < 8; i++ )
    {
        // Put a space in letter 5
        if ( i != 4 )
        {
            // Generate a random letter
            int iChar = rand () % 36 + 'A';
            if ( iChar > 'Z' )
            {
                iChar = iChar - 43;
            }

            // Put it in the plate
            m_szRegPlate [i] = iChar;
        }
    }

    // Null terminator and space in number 5
    m_szRegPlate [4] = ' ';
    m_szRegPlate [8] = 0;
}


void CVehicle::SetPaintjob ( unsigned char ucPaintjob )
{
    m_ucPaintjob = ucPaintjob;
    m_Color.SetColor1 ( 1 );
    m_Color.SetColor2 ( 1 );
    m_Color.SetColor3 ( 1 );
    m_Color.SetColor4 ( 1 );
}


void CVehicle::GetInitialDoorStates ( unsigned char * pucDoorStates )
{
    switch ( m_usModel )
    {
        case VT_BAGGAGE:
        case VT_BANDITO:
        case VT_BFINJECT:
        case VT_CADDY:
        case VT_DOZER:
        case VT_FORKLIFT:
        case VT_KART:
        case VT_MOWER:
        case VT_QUAD:
        case VT_RCBANDIT:
        case VT_RCCAM:
        case VT_RCGOBLIN:
        case VT_RCRAIDER:
        case VT_TIGER:
        case VT_TRACTOR:
        case VT_VORTEX:
            memset ( pucDoorStates, DT_DOOR_MISSING, 6 );

            // Keep the bonet and boot intact
            pucDoorStates [ 0 ] = pucDoorStates [ 1 ] = DT_DOOR_INTACT;
            break;        
        default:
            memset ( pucDoorStates, DT_DOOR_INTACT, 6 );
    }
}
