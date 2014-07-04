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
void CPlayerInfoSA::SetPlayerMoney ( long lMoney, bool bInstant )
{
    DEBUG_TRACE("void CPlayerInfoSA::SetPlayerMoney ( unsigned long ulMoney, bool bInstant )");
    MemPutFast < long > ( 0xB7CE50, lMoney );

    if (bInstant)
        MemPutFast < long >(0xB7CE54, lMoney);
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
