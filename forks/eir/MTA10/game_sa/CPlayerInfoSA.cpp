/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerInfoSA.cpp
*  PURPOSE:     Player ped type information
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/*=========================================================
    GetPlayerPed

    Arguments:
        id - index of the player to get the ped of
    Purpose:
        Returns the ped associated with the player id.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E210
=========================================================*/
CPlayerPedSAInterface* __cdecl GetPlayerPed( int id )
{
    return PlayerInfo::GetInfo( id ).pPed;
}

/*=========================================================
    GetPlayerVehicle

    Arguments:
        id - index of the player
        excludeRemote - return NULL if controlling RC vehicle
    Purpose:
        Returns the player vehicle.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E0D0
=========================================================*/
CVehicleSAInterface* __cdecl GetPlayerVehicle( int id, bool excludeRemote )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    CPlayerPedSAInterface *ped = info.pPed;

    if ( !ped || !ped->pedFlags.bInVehicle )
        return NULL;

    if ( excludeRemote && info.pRemoteVehicle )
        return NULL;

    return (CVehicleSAInterface*)ped->CurrentObjective;
}

/*=========================================================
    FindPlayerCoords

    Arguments:
        pos - vector pointer to write position into
        id - player index
    Purpose:
        Writes the player given by id into pos. If we are inside
        of the streaming update routine, then we write the center
        of world instead. Returns the output vector.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E010
=========================================================*/
const CVector& __cdecl FindPlayerCoords( CVector& pos, int id )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    CPlayerPedSAInterface *ped = info.pPed;

    // If there is no player ped loaded yet, we
    // default to 0,0,0
    if ( !ped )
    {
        pos = CVector( 0, 0, 0 );
        return pos;
    }

    const CVector *entityPos = NULL;

    // If the player is inside of a vehicle,
    // use its position instead.
    if ( ped->pedFlags.bInVehicle )
    {
        CEntitySAInterface *veh = ped->CurrentObjective;

        if ( veh )
            entityPos = &veh->Placeable.GetPosition();
    }

    // If there was no valid position set (i.e. vehicle),
    // we default to ped position.
    if ( !entityPos )
        entityPos = &ped->Placeable.GetPosition();

    pos = *entityPos;
    return pos;
}

/*=========================================================
    FindPlayerCenterOfWorld

    Arguments:
        id - player index to get the center of
    Purpose:
        Returns the vector which describes the exact center
        of world. If we have set a static center of world,
        use it instead.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E250
=========================================================*/
inline const CEntitySAInterface* GetPlayerEntityContext( int id )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    // We check the remote vehicle first.
    if ( CVehicleSAInterface *remoteVehicle = info.pRemoteVehicle )
        return remoteVehicle;

    // Let us try the player's vehicle next.
    if ( CVehicleSAInterface *veh = GetPlayerVehicle( id, false ) )
        return veh;

    // Default to the player.
    return info.pPed;
}

const CVector& __cdecl FindPlayerCenterOfWorld( int id )
{
    // MTA fix: check for static center of world and return
    // it if set.
    if ( World::IsCenterOfWorldSet() )
        return World::GetCenterOfWorld();

    // If the GTA:SA static camera is active, we report back
    // the camera position.
    if ( *(bool*)0x009690C1 )
        return Camera::GetInterface().Placeable.GetPosition();

    // Get the position of the entity context.
    return GetPlayerEntityContext( id )->Placeable.GetPosition();
}

/*=========================================================
    FindPlayerHeading

    Arguments:
        id - player index to get the heading of
    Purpose:
        Returns the floating point heading value active for
        the player. If the center of world is set, return
        a false heading instead.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E450
=========================================================*/
float __cdecl FindPlayerHeading( int id )
{
    // MTA fix: return a false heading if center of world
    // is set.
    if ( World::IsCenterOfWorldSet() )
        return World::GetFalseHeading();

    return GetPlayerEntityContext( id )->Placeable.GetHeading();
}

/*=========================================================
    SetupPlayerPed

    Arguments:
        playerIndex - slot to save the player ped into
    Purpose:
        Creates a new player ped and sets it at the given slot.
        The slot is an entry in the global playerInfo structure.
        Natively, GTA:SA supports only slot 0.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0060D790
=========================================================*/
void __cdecl SetupPlayerPed( unsigned int playerIndex )
{
    CPlayerPedSAInterface *player = (CPlayerPedSAInterface*)( (*ppPedPool)->Allocate() );

    // We should be able to create players natively someday.
    // Until then, here is an ugly ASM chunk.
    // It calls the player constructor.
    __asm
    {
        mov ecx,player
        push 0
        push playerIndex
        mov eax,0x0060D5B0
        call eax
    }

    CPlayerInfoSAInterface& playerInfo = PlayerInfo::GetInfo( playerIndex );
    playerInfo.pPed = player;

    if ( playerIndex == 1 )
        player->bPedType = PEDTYPE_PLAYER2;

    player->SetOrientation( 0, 0, 0 );

    World::AddEntity( player );

    player->m_unkPlayerVal = 100;

    playerInfo.PlayerState = PS_PLAYING;
}

/*=========================================================
    HOOK_CRunningScript_Process

    Purpose:
        Used by the game to execute .SCM scripts. We use it
        to manage the player entity.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E450
=========================================================*/
static bool bHasProcessedScript = false;

void __cdecl HOOK_CRunningScript_Process( void )
{
    if ( !bHasProcessedScript )
    {
        CCameraSAInterface& camera = Camera::GetInterface();
        camera.SetFadeColor( 0, 0, 0 );
        camera.Fade( 0.0f, FADE_OUT );

        Streaming::RequestSpecialModel( 0, "player", 26 );
        Streaming::LoadAllRequestedModels( true );

        SetupPlayerPed( 0 );

        PlayerInfo::GetInfo( 0 ).pPed->Placeable.SetPosition( 0, 0, 0 );

        // We have set up our player instance.
        bHasProcessedScript = true;
    }
}

void PlayerInfo_Init( void )
{
    // Hook fixes.
    HookInstall( 0x0056E010, (DWORD)FindPlayerCoords, 5 );
    HookInstall( 0x0056E250, (DWORD)FindPlayerCenterOfWorld, 5 );
    HookInstall( 0x0056E450, (DWORD)FindPlayerHeading, 5 );
    HookInstall( 0x00469F00, (DWORD)HOOK_CRunningScript_Process, 5 );
}

void PlayerInfo_Shutdown( void )
{

}

/**
 * Gets the Player Ped for the local player
 * @return CPlayerPed * for the local player
 */
CPlayerPed * CPlayerInfoSA::GetPlayerPed (  )
{
    DEBUG_TRACE("CPlayerPed * CPlayerInfoSA::GetPlayerPed (  )");
    return (CPlayerPed *)VAR_PlayerPed;
}

CWanted * CPlayerInfoSA::GetWanted ( )
{
    DEBUG_TRACE("CWanted * CPlayerInfoSA::GetWanted ( )");

    if ( !wanted )
        wanted = new CWantedSA(this->internalInterface->PlayerPedData.m_Wanted); 

    return wanted;
}

void CPlayerInfoSA::GetCrossHair ( bool &bActivated, float &fTargetX, float &fTargetY )
{
    DEBUG_TRACE("void CPlayerInfoSA::GetCrossHair ( bool &bEnabled, float &fTargetX, float &fTargetY )");
    bActivated = internalInterface->CrossHair.bActivated;
    fTargetX = internalInterface->CrossHair.TargetX;
    fTargetY = internalInterface->CrossHair.TargetY;
}

/**
 * Gets the amount of money the player has
 * @return DWORD containing the ammount of money the player has
 */
long CPlayerInfoSA::GetPlayerMoney (  )
{
    DEBUG_TRACE("unsigned long CPlayerInfoSA::GetPlayerMoney ( void )");
    //return internalInterface->DisplayScore;
    return * ( long * ) ( 0xB7CE50 );
}

/**
 * Sets the amount of money the player has
 * @param dwMoney DWORD containing the ammount of money you wish the player to have
 */
void CPlayerInfoSA::SetPlayerMoney ( long lMoney )
{
    DEBUG_TRACE("void CPlayerInfoSA::SetPlayerMoney ( unsigned long ulMoney )");
    //internalInterface->DisplayScore = ulMoney;
    MemPutFast < long > ( 0xB7CE50, lMoney );
}

/**
 *
 */
VOID CPlayerInfoSA::GivePlayerParachute ( VOID )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::GivePlayerParachute ( VOID )");
    DWORD dwFunction = FUNC_GivePlayerParachute;
    _asm {
        call dwFunction
    }
}

/**
 *
 */
VOID CPlayerInfoSA::StreamParachuteWeapon ( bool bAllowParachute )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::StreamParachuteWeapon ( bool bAllowParachute )");
    DWORD dwFunction = FUNC_StreamParachuteWeapon;
    _asm {
        push bAllowParachute
        call dwFunction
    }
}

/**
 * Not really sure what this does?
 * \todo Find out what this function actually does.
 * @param boolSafe TRUE if the player is to be made safe, FALSE to make them unsafe
 */
VOID CPlayerInfoSA::MakePlayerSafe ( BOOL boolSafe )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::MakePlayerSafe ( BOOL boolSafe )");
    DWORD dwFunction = FUNC_MakePlayerSafe;
    _asm
    {
        push    boolSafe
        call    dwFunction
    }
}

/**
 * Stop the player entering a specific car
 * @param vehicle The vehicle you want to stop them entering
 * \todo Find out what this really does...
 */
VOID CPlayerInfoSA::CancelPlayerEnteringCars ( CVehicle * vehicle )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::CancelPlayerEnteringCars ( CVehicle * vehicle )");
    DWORD dwFunction = FUNC_CancelPlayerEnteringCars;
    _asm
    {
        push    vehicle
        call    dwFunction
    }
}

/**
 * Make the player arrested - displays "Busted" text are they are respawned at the nearest police station
 * @see CRestartSA::OverrideNextRestart
 * @see CRestartSA::AddPoliceRestartPoint
 */
VOID CPlayerInfoSA::ArrestPlayer (  )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::ArrestPlayer (  )");
    DWORD dwFunction = FUNC_ArrestPlayer;
    _asm
    {
        call    dwFunction
    }
}

/**
 * Kill the player - displays "Wasted" text are they are respawned at the nearest hospital station
 * @see CRestartSA::OverrideNextRestart
 * @see CRestartSA::AddHospitalRestartPoint
 */
VOID CPlayerInfoSA::KillPlayer (  )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::KillPlayer (  )");
    DWORD dwFunction = FUNC_KillPlayer;
    _asm
    {
        call    dwFunction
    }
}


/**
 * Creates a remote controlled vehicle of a specific type at a specified point
 * @param vehicletype This is the type of vehicle to create
 * @return CVehicle * for the created remote controlled vehicle
 */
CVehicle * CPlayerInfoSA::GivePlayerRemoteControlledCar ( eVehicleTypes vehicletype )
{
    DEBUG_TRACE("CVehicle * CPlayerInfoSA::GivePlayerRemoteControlledCar ( eVehicleTypes vehicletype )");
    /**
     * \todo Add the player's X, Y, Z as the initial start position
     */
    DWORD dwFunction = FUNC_GivePlayerRemoteControlledCar;
    //this->GetPlayerPed();
    float fX, fY, fZ;
    float fRotation;
    _asm
    {
        push    vehicletype
        push    fRotation;
        push    fZ
        push    fY
        push    fX
        call    dwFunction
        add     esp, 0x14
    }

    return this->GetPlayerRemoteControlledCar();

}

/**
 * Stops the player from controlling an RC vehicle
 */
VOID CPlayerInfoSA::TakeRemoteControlledCarFromPlayer (  )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::TakeRemoteControlledCarFromPlayer (  )");
    DWORD dwFunction = FUNC_TakeRemoteControlledCarFromPlayer;
    _asm
    {
        push    ecx
        push    1
        call    dwFunction
        pop     ecx
    }
}

/**
 * Get the car that the player is controlling remotely
 * @return CVehicle * containing the relevant vehicle, or NULL if the player isn't controlling a vehicle
 */
CVehicle * CPlayerInfoSA::GetPlayerRemoteControlledCar (  )
{
    DEBUG_TRACE("CVehicle * CPlayerInfoSA::GetPlayerRemoteControlledCar (  )");
    return (CVehicle *)VAR_PlayerRCCar;
}

float CPlayerInfoSA::GetFPSMoveHeading ( void )
{
    return this->GetInterface()->PlayerPedData.m_fFPSMoveHeading;
}


bool CPlayerInfoSA::GetDoesNotGetTired ( void )
{
    return internalInterface->DoesNotGetTired;
}


void CPlayerInfoSA::SetDoesNotGetTired ( bool bDoesNotGetTired )
{
    internalInterface->DoesNotGetTired = bDoesNotGetTired;
}


short CPlayerInfoSA::GetLastTimeEaten ( void )
{
    return internalInterface->TimeLastEaten;
}


void CPlayerInfoSA::SetLastTimeEaten ( short sTime )
{
    internalInterface->TimeLastEaten = sTime;
}


DWORD CPlayerInfoSA::GetLastTimeBigGunFired ( void )
{
    return internalInterface->LastTimeBigGunFired;
}


void CPlayerInfoSA::SetLastTimeBigGunFired ( DWORD dwTime )
{
    internalInterface->LastTimeBigGunFired = dwTime;
}
