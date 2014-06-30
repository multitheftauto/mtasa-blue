/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayer_keysync.cpp
*  PURPOSE:     Multiplayer module keysync methods
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define MULTIPLAYER_STATS

// These includes have to be fixed!
#include "../game_sa/CPlayerInfoSA.h"
#include "../game_sa/CPedSA.h"
#include "../game_sa/CVehicleSA.h"
#include "../game_sa/CPadSA.h"

extern CMultiplayerSA* pMultiplayer;

DWORD dwCurrentPlayerPed = 0; // stores the player ped temporarily during hooks
DWORD dwCurrentVehicle = 0; // stores the current vehicle during the hooks

DWORD dwParameter = 0;

BOOL bRadioHackInstalled = FALSE;
bool b1stPersonWeaponModeHackInPlace = false;

/*
GTA_CONTROLSET RemotePlayerKeys[MAX_PEDS];
GTA_CONTROLSET SavedLocalPlayerKeys;
GTA_CONTROLSET *LocalPlayerKeys = (GTA_CONTROLSET *)VAR_Keystates;
*/

bool bNotInLocalContext = false;
bool bMouseLookEnabled = true;
bool bInfraredVisionEnabled = false;
bool bNightVisionEnabled = false;

extern CStatsData localStatsData;
extern bool bLocalStatsStatic;
extern float fLocalPlayerCameraRotation;
extern bool bCustomCameraRotation;
extern float fGlobalGravity;
extern float fLocalPlayerGravity;

extern PreContextSwitchHandler* m_pPreContextSwitchHandler;
extern PostContextSwitchHandler* m_pPostContextSwitchHandler;

#define HOOKPOS_CAudioEngine_Service                        0x50778C
DWORD RETURN_CAudioEngine_Service_Cont                    = 0x507791;
DWORD RETURN_CAudioEngine_Service_Fix                     = 0x50780A;

#define HOOKPOS_CAEVehicleAudioEntity_Service               0x502284
DWORD RETURN_CAEVehicleAudioEntity_Service_Cont           = 0x50228C;
DWORD RETURN_CAEVehicleAudioEntity_Service_Fix            = 0x5023C5;

void HOOK_CAudioEngine_Service ( );
void HOOK_CAEVehicleAudioEntity_Service ( );

VOID InitKeysyncHooks()
{
    //OutputDebugString("InitKeysyncHooks");
    HookInstallMethod(VTBL_CPlayerPed__ProcessControl,      (DWORD)HOOK_CPlayerPed__ProcessControl);
    HookInstallMethod(VTBL_CAutomobile__ProcessControl,     (DWORD)HOOK_CAutomobile__ProcessControl);
    HookInstallMethod(VTBL_CMonsterTruck__ProcessControl,   (DWORD)HOOK_CMonsterTruck__ProcessControl);
    HookInstallMethod(VTBL_CTrailer__ProcessControl,        (DWORD)HOOK_CTrailer__ProcessControl);
    HookInstallMethod(VTBL_CQuadBike__ProcessControl,       (DWORD)HOOK_CQuadBike__ProcessControl);
    HookInstallMethod(VTBL_CPlane__ProcessControl,          (DWORD)HOOK_CPlane__ProcessControl);
    HookInstallMethod(VTBL_CBmx__ProcessControl,            (DWORD)HOOK_CBmx__ProcessControl);
    HookInstallMethod(VTBL_CTrain__ProcessControl,          (DWORD)HOOK_CTrain__ProcessControl);
    HookInstallMethod(VTBL_CBoat__ProcessControl,           (DWORD)HOOK_CBoat__ProcessControl);
    HookInstallMethod(VTBL_CBike__ProcessControl,           (DWORD)HOOK_CBike__ProcessControl);
    HookInstallMethod(VTBL_CHeli__ProcessControl,           (DWORD)HOOK_CHeli__ProcessControl);
    HookInstallMethod(VTBL_CHeli__ProcessControl,           (DWORD)HOOK_CHeli__ProcessControl);
    
    // not strictly for keysync, to make CPlayerPed::GetPlayerInfoForThisPlayerPed always return the local playerinfo
    //00609FF2     EB 1F          JMP SHORT gta_sa_u.0060A013
    MemSet ((void *)0x609FF2, 0xEB, 1);
    MemSet ((void *)0x609FF3, 0x1F, 1);
    MemSet ((void *)0x609FF4, 0x90, 1);
    MemSet ((void *)0x609FF5, 0x90, 1);
    MemSet ((void *)0x609FF6, 0x90, 1);
    
    // and this is to fix bike sync (I hope)
    //006BC9EB   9090               NOP NOP
    MemSet ((void *)0x6BC9EB, 0x90, 2);

    HookInstall ( HOOKPOS_CAudioEngine_Service, (DWORD) HOOK_CAudioEngine_Service, 5 );
    HookInstall ( HOOKPOS_CAEVehicleAudioEntity_Service, (DWORD) HOOK_CAEVehicleAudioEntity_Service, 6 );
}

extern CPed* pContextSwitchedPed;
void PostContextSwitch ( void )
{
    // Prevent the game making remote player's weapons get switched by the local player's
    MemPutFast < BYTE > ( 0x60D850, 0x56 );
    MemPutFast < BYTE > ( 0x60D851, 0x57 );
    MemPutFast < BYTE > ( 0x60D852, 0x8B );

    // Prevent it calling ClearWeaponTarget for remote players
    MemPutFast < BYTE > ( 0x609C80, 0x57 );

    // Prevent CCamera::SetNewPlayerWeaponMode being called
    MemPutFast < BYTE > ( 0x50BFB0, 0x66 );
    MemPutFast < BYTE > ( 0x50BFB1, 0x8B );
    MemPutFast < BYTE > ( 0x50BFB2, 0x44 );

    // This is so weapon clicks and similar don't play for us when done remotly
    MemPutFast < BYTE > ( 0x60F273, 0x75 );
    MemPutFast < BYTE > ( 0x60F260, 0x74 );
    MemPutFast < BYTE > ( 0x60F261, 0x13 );

    // Prevent it calling CCamera::ClearPlayerWeaponMode for remote players
    MemPutFast < BYTE > ( 0x50AB10, 0x33 );

    // this is to prevent shooting players following the local camera
    MemPutFast < BYTE > ( 0x687099, 0x75 );

    // Prevent rockets firing oddly
    //*(BYTE *)0x73811C = 0x0F;
    //*(BYTE *)0x73811D = 0x84;

    // Prevent it marking targets of remote players
    MemPutFast < BYTE > ( 0x742BF0, 0x8B );

    // Restore the mouse look state back to the default
    MemPutFast < bool > ( 0xB6EC2E, bMouseLookEnabled );

    // Restore the visual goggle mode back
    MemPutFast < bool > ( 0xC402B9, bInfraredVisionEnabled );
    MemPutFast < bool > ( 0xC402B8, bNightVisionEnabled );

    // Make players cough on fire extinguisher and teargas again
    MemPutFast < unsigned char > ( 0x4C03F0, 0x83 );
    MemPutFast < unsigned char > ( 0x4C03F1, 0xF8 );
    MemPutFast < unsigned char > ( 0x4C03F2, 0x29 );
    MemPutFast < unsigned char > ( 0x4C03F8, 0x74 );
    MemPutFast < unsigned char > ( 0x4C03F9, 0x09 );
    MemPutFast < unsigned char > ( 0x4C03FA, 0x83 );
    MemPutFast < unsigned char > ( 0x4C03FB, 0xF8 );
    MemPutFast < unsigned char > ( 0x4C03FC, 0x2A );
    MemPutFast < unsigned char > ( 0x4C03FD, 0x74 );
    MemPutFast < unsigned char > ( 0x4C03FE, 0x04 );

    // make the CCamera::Using1stPersonWeaponMode function return true
    if ( b1stPersonWeaponModeHackInPlace)
    {
        b1stPersonWeaponModeHackInPlace = false;

        MemPutFast < BYTE > ( 0x50BFF0, 0x66 );
        MemPutFast < BYTE > ( 0x50BFF1, 0x8B );
        MemPutFast < BYTE > ( 0x50BFF2, 0x81 );
    }


    if ( bRadioHackInstalled )
    {
        // For tanks, to prevent our mouse movement affecting remote tanks
        // 006AEA25   0F85 60010000    JNZ gta_sa.006AEB8B
        // ^
        // 006AEA25   90               NOP
        // 006AEA26   E9 60010000      JMP gta_sa.006AEB8B
        MemPutFast < BYTE > ( 0x6AEA25, 0x0F );
        MemPutFast < BYTE > ( 0x6AEA26, 0x85 );

        // Same for firetrucks and SWATs
        // 00729B96   0F85 75010000    JNZ gta_sa.00729D11
        // ^
        // 00729B96   90               NOP
        // 00729B97   E9 75010000      JMP gta_sa.00729D11
        MemPutFast < BYTE > ( 0x729B96, 0x0F );
        MemPutFast < BYTE > ( 0x729B97, 0x85 );

        bRadioHackInstalled = FALSE;
    }

    
    // Prevent the game making remote players vehicle's audio behave like locals (and deleting 
    // radio etc when they are removed) - issue #95
    MemPutFast < BYTE > ( 0x50230C, 0x1 );

    // Re-Enable 0x5022C9 - PlayerAboutToExitVehicleAsDriver
    MemPut < BYTE > ( 0x5022C9, 0xE8 );
    MemPut < BYTE > ( 0x5022CA, 0xD2 );
    MemPut < BYTE > ( 0x5022CB, 0x38 );
    MemPut < BYTE > ( 0x5022CC, 0xFF );
    MemPut < BYTE > ( 0x5022CD, 0xFF );

    // Re-Enable 0x5022ED - JustWreckedVehicle
    MemPut < BYTE > ( 0x5022ED, 0xE8 );
    MemPut < BYTE > ( 0x5022EE, 0xBE );
    MemPut < BYTE > ( 0x5022EF, 0xAD );
    MemPut < BYTE > ( 0x5022F0, 0xFF );
    MemPut < BYTE > ( 0x5022F1, 0xFF );

    // Re-Enable 0x4EB9A0 - CAERadioTrackManager::Service
    MemPut < BYTE > ( 0x4EB9A0, 0x55 ); 

    // Fix our JNZ/JZ's - these are inverted such that is player is considered false
    MemPut < BYTE > ( 0x6420F8, 0x75 );
    MemPut < BYTE > ( 0x648A2A, 0x75 );
    MemPut < BYTE > ( 0x64CA98, 0x74 );
    MemPut < BYTE > ( 0x5022C5, 0x75 );

    /*
    // ChrML: Force as high stats as we can go before screwing up. Players can't have different
    // stats or guns don't work. We can't have dual guns either due to some screwups.
    // Dual gun screwup: Sync code needs update and the gun pointing up needs to.
    localStatsData.StatTypesFloat [ 69 ] = 500.0f;
    localStatsData.StatTypesFloat [ 70 ] = 999.0f;
    localStatsData.StatTypesFloat [ 71 ] = 999.0f;
    localStatsData.StatTypesFloat [ 72 ] = 999.0f;
    localStatsData.StatTypesFloat [ 73 ] = 500.0f;
    localStatsData.StatTypesFloat [ 74 ] = 999.0f;
    localStatsData.StatTypesFloat [ 75 ] = 500.0f;
    localStatsData.StatTypesFloat [ 76 ] = 999.0f;
    localStatsData.StatTypesFloat [ 77 ] = 999.0f;
    localStatsData.StatTypesFloat [ 78 ] = 999.0f;
    localStatsData.StatTypesFloat [ 79 ] = 999.0f;
    */

    // ChrML: This causes the aiming issues
    // Restore the local player stats    
    MemCpyFast ( (void *)0xb79380, &localStatsData.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
    MemCpyFast ( (void *)0xb79000, &localStatsData.StatTypesInt, sizeof(int) * MAX_INT_STATS );
    MemCpyFast ( (void *)0xb78f10, &localStatsData.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );
}

VOID ReturnContextToLocalPlayer()
{   
    if ( bNotInLocalContext )
    {
        // Grab the remote data storage for the player we context switched to
        CPlayerPed* pContextSwitchedPlayerPed = dynamic_cast < CPlayerPed* > ( pContextSwitchedPed );
        if ( pContextSwitchedPlayerPed )
        {
            CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( pContextSwitchedPlayerPed );
            if ( data )
            {
                // Store any changes the game has made to the pad
                CPad* pLocalPad = pGameInterface->GetPad ();
                CPadSAInterface* pLocalPadInterface = ( (CPadSA*) pLocalPad )->GetInterface ();
                MemCpyFast ( &data->m_pad, pLocalPadInterface, sizeof ( CPadSAInterface ) );            
            }
        }

        pGameInterface->GetPad()->Restore();
        
        MemPutFast < float > ( VAR_CameraRotation, fLocalPlayerCameraRotation );

        bNotInLocalContext = false;

        CPed* pLocalPlayerPed = pGameInterface->GetPools ()->GetPedFromRef ( (DWORD)1 ); // the player
        CPedSA* pLocalPlayerPedSA = dynamic_cast < CPedSA* > ( pLocalPlayerPed );
        if ( pLocalPlayerPedSA )
        {
            CEntitySAInterface * ped = pLocalPlayerPedSA->GetInterface();
            MemPutFast < DWORD > ( 0xB7CD98, (DWORD)ped );
        }


        PostContextSwitch();
        pGameInterface->OnPedContextChange ( NULL );

        if ( m_pPostContextSwitchHandler )
        {
            m_pPostContextSwitchHandler ();
        }        
    }
    else
    {
        // Store any changes to the local-players stats?
        if ( !bLocalStatsStatic )
        {
            assert ( 0 );   // bLocalStatsStatic is always true
            MemCpyFast ( &localStatsData.StatTypesFloat, (void *)0xb79380, sizeof(float) * MAX_FLOAT_STATS );
            MemCpyFast ( &localStatsData.StatTypesInt, (void *)0xb79000, sizeof(int) * MAX_INT_STATS );
            MemCpyFast ( &localStatsData.StatReactionValue, (void *)0xb78f10, sizeof(float) * MAX_REACTION_STATS );
        }
    }

    // We need to set this back, even if its the local player
    pGameInterface->SetGravity ( fGlobalGravity );
}

void SwitchContext ( CPed* thePed )
{
    if ( thePed == NULL ) return;

    pContextSwitchedPed = thePed;
    // Are we not already in another context?
    if ( !bNotInLocalContext )
    {
        // Grab the local ped and the local pad
        CPed* pLocalPlayerPed = pGameInterface->GetPools ()->GetPedFromRef ( (DWORD)1 ); // the player
        CPad* pLocalPad = pGameInterface->GetPad ();
        CPadSAInterface* pLocalPadInterface = ( (CPadSA*) pLocalPad )->GetInterface ();

        // We're not switching to local player
        if ( thePed != pLocalPlayerPed )
        {
            // Store the local pad
            pLocalPad->Store (); // store a copy of the local pad internally

            // Grab the remote data storage for the player we're context switching to
            CPlayerPed* thePlayerPed = dynamic_cast < CPlayerPed* > ( thePed );
            if ( thePlayerPed )
            {
                CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( thePlayerPed );
                if ( data )
                {
                    // We want the player to be seen as in targeting mode if they are right clicking and with weapons 
                    CWeapon* pWeapon = thePed->GetWeapon(thePed->GetCurrentWeaponSlot());
                    eWeaponType currentWeapon = pWeapon->GetType();
                    CControllerState * cs = data->CurrentControllerState();
                    CWeaponStat * pWeaponStat = NULL;
                    if ( currentWeapon >= WEAPONTYPE_PISTOL && currentWeapon <= WEAPONTYPE_TEC9 )
                    {
                        float fValue = data->m_stats.StatTypesFloat [ pGameInterface->GetStats ()->GetSkillStatIndex ( currentWeapon ) ];
                        pWeaponStat = pGameInterface->GetWeaponStatManager ( )->GetWeaponStatsFromSkillLevel ( currentWeapon, fValue );
                    }
                    else
                        pWeaponStat = pGameInterface->GetWeaponStatManager ( )->GetWeaponStats ( currentWeapon );

                    if ( cs->RightShoulder1 != 0 
                        && ( pWeaponStat && pWeaponStat->IsFlagSet ( WEAPONTYPE_FIRSTPERSON ) ) )
                    {
                        b1stPersonWeaponModeHackInPlace = true;
                        
                        // make the CCamera::Using1stPersonWeaponMode function return true
                        MemPutFast < BYTE > ( 0x50BFF0, 0xB0 );
                        MemPutFast < BYTE > ( 0x50BFF1, 0x01 );
                        MemPutFast < BYTE > ( 0x50BFF2, 0xC3 );
                    }

                    // Change the local player's pad to the remote player's
                    MemCpyFast ( pLocalPadInterface, &data->m_pad, sizeof ( CPadSAInterface ) );

                    // this is to fix the horn/siren
                    pLocalPad->SetHornHistoryValue ( ( cs->ShockButtonL == 255 ) );
                    // disables the impatient actions on remote players (which cause desync)
                    pLocalPad->SetLastTimeTouched ( pGameInterface->GetSystemTime () );

                    // this is to make movement work correctly
                    fLocalPlayerCameraRotation = *(float *)VAR_CameraRotation;
                    MemPutFast < float > ( VAR_CameraRotation, data->m_fCameraRotation );

                    // Change the gravity to the remote player's
                    pGameInterface->SetGravity ( data->m_fGravity );

                    // Disable mouselook for remote players (so the mouse doesn't affect them)
                    // Only disable mouselook if they're not holding a 1st-person weapon
                    // And if they're not under-water
                    bool bDisableMouseLook = true;
                    if ( pWeapon )
                    {
                        eWeaponType weaponType = pWeapon->GetType ();
                        if ( pWeaponStat->IsFlagSet ( WEAPONTYPE_FIRSTPERSON ) )
                        {
                            bDisableMouseLook = false;
                        }
                    }
                    bMouseLookEnabled = *(bool *)0xB6EC2E;
                    if ( bDisableMouseLook ) *(bool *)0xB6EC2E = false;

                    // Disable the goggles
                    bInfraredVisionEnabled = *(bool *)0xC402B9;
                    MemPutFast < bool > ( 0xC402B9, false );
                    bNightVisionEnabled = *(bool *)0xC402B8;
                    MemPutFast < bool > ( 0xC402B8, false );

                    // Remove the code making players cough on fire extinguisher and teargas
                    MemSetFast ( (void*) 0x4C03F0, 0x90, 3 );
                    MemSetFast ( (void*) 0x4C03F8, 0x90, 7 );

                    // Prevent it calling ClearWeaponTarget for remote players
                    MemPutFast < BYTE > ( 0x609C80, 0xC3 );

                    // Prevent rockets firing oddly
                    //*(BYTE *)0x73811C = 0x90;
                    //*(BYTE *)0x73811D = 0xE9;

                    
                    // This is so weapon clicks and similar don't play for us when done remotly
                    MemPutFast < BYTE > ( 0x60F273, 0xEB );
                    MemPutFast < BYTE > ( 0x60F260, 0x90 );
                    MemPutFast < BYTE > ( 0x60F261, 0x90 );
                    

                    // Prevent CCamera::SetNewPlayerWeaponMode being called
                    MemPutFast < BYTE > ( 0x50BFB0, 0xC2 );
                    MemPutFast < BYTE > ( 0x50BFB1, 0x0C );
                    MemPutFast < BYTE > ( 0x50BFB2, 0x00 );

                    // Prevent it calling CCamera::ClearPlayerWeaponMode for remote players
                    MemPutFast < BYTE > ( 0x50AB10, 0xC3 );

                    // Prevent it marking targets of remote players
                    MemPutFast < BYTE > ( 0x742BF0, 0xC3 );

                    // this is to prevent shooting players following the local camera
                    MemPutFast < BYTE > ( 0x687099, 0xEB );

                    // Prevent the game making remote player's weapons get switched by the local player's
                    MemPutFast < BYTE > ( 0x60D850, 0xC2 );
                    MemPutFast < BYTE > ( 0x60D851, 0x04 );
                    MemPutFast < BYTE > ( 0x60D852, 0x00 );

                    // Change the local player's stats to the remote player's
                    if ( data )
                    {
                        MemCpyFast ( (void *)0xb79380, data->m_stats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
                        MemCpyFast ( (void *)0xb79000, data->m_stats.StatTypesInt, sizeof(int) * MAX_INT_STATS );
                        MemCpyFast ( (void *)0xb78f10, data->m_stats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );
                    }                 

                    /*
                    // ChrML: Force as high stats as we can go before screwing up. Players can't have different
                    //        stats or guns don't work. We can't have dual guns either due to some screwups.
                    //        Dual gun screwup: Sync code needs update and the gun pointing up needs to.
                    float* pfStats = (float*) 0xb79380;
                    pfStats [ 69 ] = 500.0f;
                    pfStats [ 70 ] = 999.0f;
                    pfStats [ 71 ] = 999.0f;
                    pfStats [ 72 ] = 999.0f;
                    pfStats [ 73 ] = 500.0f;
                    pfStats [ 74 ] = 999.0f;
                    pfStats [ 75 ] = 500.0f;
                    pfStats [ 76 ] = 999.0f;
                    pfStats [ 77 ] = 999.0f;
                    pfStats [ 78 ] = 999.0f;
                    pfStats [ 79 ] = 999.0f;
                    */

                    // Prevent the game making remote players vehicle's audio behave like locals (and deleting 
                    // radio etc when they are removed) - issue #95
                    MemPutFast < BYTE > ( 0x50230C, 0x0 );

                    // Disable 0x5022C9 - PlayerAboutToExitVehicleAsDriver
                    MemPut < BYTE > ( 0x5022C9, 0x90 );
                    MemPut < BYTE > ( 0x5022CA, 0x90 );
                    MemPut < BYTE > ( 0x5022CB, 0x90 );
                    MemPut < BYTE > ( 0x5022CC, 0x90 );
                    MemPut < BYTE > ( 0x5022CD, 0x90 );

                    // Disable 0x5022ED - JustWreckedVehicle
                    MemPut < BYTE > ( 0x5022ED, 0x90 );
                    MemPut < BYTE > ( 0x5022EE, 0x90 );
                    MemPut < BYTE > ( 0x5022EF, 0x90 );
                    MemPut < BYTE > ( 0x5022F0, 0x90 );
                    MemPut < BYTE > ( 0x5022F1, 0x90 );
                    
                    // Disable 0x4EB9A0 - CAERadioTrackManager::Service
                    MemPut < BYTE > ( 0x4EB9A0, 0xC2 );
                    
                    // Invert some JNZ/JZ's - these are inverted such that is player is considered false
                    MemPut < BYTE > ( 0x6420F8, 0x74 );
                    MemPut < BYTE > ( 0x648A2A, 0x74 );
                    MemPut < BYTE > ( 0x64CA98, 0x75 );
                    MemPut < BYTE > ( 0x5022C5, 0x74 );

                    CPedSA* thePedSA = dynamic_cast < CPedSA* > ( thePed );
                    if ( thePedSA )
                    {
                        CEntitySAInterface * ped = thePedSA->GetInterface();
                        MemPutFast < DWORD > ( 0xB7CD98, (DWORD)ped );
                    }

                    // Remember that we're not in the local player's context any more (for switching back)
                    bNotInLocalContext = true;

                    // Call the pre-context switch handler we might have
                    if ( m_pPreContextSwitchHandler )
                    {
                        CPlayerPed* pPlayerPed = dynamic_cast < CPlayerPed* > ( thePed );
                        if ( pPlayerPed )
                            m_pPreContextSwitchHandler ( pPlayerPed );
                    }
                }
            }
        }
        else
        {
            // Set the local players gravity
            pGameInterface->SetGravity ( fLocalPlayerGravity );

            if ( bCustomCameraRotation )
                MemPutFast < float > ( VAR_CameraRotation, fLocalPlayerCameraRotation );
        }
    }
    pGameInterface->OnPedContextChange ( thePed );
}


void SwitchContext ( CPedSAInterface* ped )
{
    CPed* thePed = pGameInterface->GetPools ()->GetPed ( (DWORD*) ped );
    if ( thePed )
    {
        SwitchContext ( thePed );
    }
}


void SwitchContext ( CVehicle* pVehicle )
{
    if ( !pVehicle ) return;

    // Grab the vehicle's internal interface
    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );

    DWORD dwVehicle = (DWORD)pVehicleSA->GetInterface ();

    // Grab the driver of the vehicle
    CPed* thePed = pVehicle->GetDriver ();
    if ( thePed )
    {
        // Switch the context to the driver of the vehiclee
        SwitchContext ( thePed );
        if ( bNotInLocalContext )
        {
            MemPutFast < BYTE > ( dwVehicle + 312 + 0xA5, 0 );

            // For tanks, to prevent our mouse movement affecting remote tanks
            // 006AEA25   0F85 60010000    JNZ gta_sa.006AEB8B
            // V
            // 006AEA25   90               NOP
            // 006AEA26   E9 60010000      JMP gta_sa.006AEB8B
            MemPutFast < BYTE > ( 0x6AEA25, 0x90 );
            MemPutFast < BYTE > ( 0x6AEA26, 0xE9 );

            // Same for firetrucks and SWATs
            // 00729B96   0F85 75010000    JNZ gta_sa.00729D11
            // V
            // 00729B96   90               NOP
            // 00729B97   E9 75010000      JMP gta_sa.00729D11
            MemPutFast < BYTE > ( 0x729B96, 0x90 );
            MemPutFast < BYTE > ( 0x729B97, 0xE9 );

            bRadioHackInstalled = TRUE;
        }
        else
        {

            //0050237C  |. E8 9F37FFFF    CALL gta_sa_u.004F5B20
            MemPutFast < BYTE > ( 0x50237C + 0, 0xE8 );
            MemPutFast < BYTE > ( 0x50237C + 1, 0x9F );
            MemPutFast < BYTE > ( 0x50237C + 2, 0x37 );
            MemPutFast < BYTE > ( 0x50237C + 3, 0xFF );
            MemPutFast < BYTE > ( 0x50237C + 4, 0xFF );

            //0x5023A3
            MemPutFast < BYTE > ( 0x5023A3 + 0, 0xE8 );
            MemPutFast < BYTE > ( 0x5023A3 + 1, 0xB8 );
            MemPutFast < BYTE > ( 0x5023A3 + 2, 0x37 );
            MemPutFast < BYTE > ( 0x5023A3 + 3, 0xFF );
            MemPutFast < BYTE > ( 0x5023A3 + 4, 0xFF );

            //0x5023E1
            MemPutFast < BYTE > ( 0x5023E1 + 0, 0xE8 );
            MemPutFast < BYTE > ( 0x5023E1 + 1, 0x1A );
            MemPutFast < BYTE > ( 0x5023E1 + 2, 0x33 );
            MemPutFast < BYTE > ( 0x5023E1 + 3, 0xFF );
            MemPutFast < BYTE > ( 0x5023E1 + 4, 0xFF );

        }
    }
}


void SwitchContext ( CVehicleSAInterface* pVehicleInterface )
{   
    // Grab the CVehicle for the given vehicle interface
    CPoolsSA* pool = (CPoolsSA*) pGameInterface->GetPools ();
    CVehicle* pVehicle = pool->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( pVehicle )
    {
        SwitchContext ( pVehicle );
    }
}

/************************** ACTUAL HOOK FUNCTIONS BELOW THIS LINE *******************************/


struct CSavedRegs
{
    DWORD eax, ecx, edx, ebx, esp, ebp, esi, edi;
};
static CSavedRegs PlayerPed__ProcessControl_Saved;


VOID _declspec(naked) HOOK_CPlayerPed__ProcessControl()
{
    // Assumes no reentrancy
    _asm
    {
        mov     dwCurrentPlayerPed, ecx

        // Save incase of abort
        mov     PlayerPed__ProcessControl_Saved.eax, eax
        mov     PlayerPed__ProcessControl_Saved.ecx, ecx
        mov     PlayerPed__ProcessControl_Saved.edx, edx
        mov     PlayerPed__ProcessControl_Saved.ebx, ebx
        mov     PlayerPed__ProcessControl_Saved.esp, esp
        mov     PlayerPed__ProcessControl_Saved.ebp, ebp
        mov     PlayerPed__ProcessControl_Saved.esi, esi
        mov     PlayerPed__ProcessControl_Saved.edi, edi
        pushad
    }

    SwitchContext((CPedSAInterface *)dwCurrentPlayerPed);

    _asm
    {
        popad
        mov     edx, FUNC_CPlayerPed__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();

    _asm
    {
        popad
        retn
    }
}


void  _declspec(naked) CPlayerPed__ProcessControl_Abort()
{
    _asm
    {
        // restore stuff
        mov     eax, PlayerPed__ProcessControl_Saved.eax
        mov     ecx, PlayerPed__ProcessControl_Saved.ecx
        mov     edx, PlayerPed__ProcessControl_Saved.edx
        mov     ebx, PlayerPed__ProcessControl_Saved.ebx
        mov     esp, PlayerPed__ProcessControl_Saved.esp
        mov     ebp, PlayerPed__ProcessControl_Saved.ebp
        mov     esi, PlayerPed__ProcessControl_Saved.esi
        mov     edi, PlayerPed__ProcessControl_Saved.edi
        pushad
    }

    ReturnContextToLocalPlayer();

    _asm
    {
        popad
        retn
    }
}


//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CAutomobile__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }
    
    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CAutomobile__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer ();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CMonsterTruck__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CMonsterTruck__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CTrailer__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CTrailer__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CQuadBike__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CQuadBike__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CPlane__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CPlane__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CBmx__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CBmx__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CTrain__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CTrain__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CBoat__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CBoat__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CBike__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CBike__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

//--------------------------------------------------------------------------------------------

VOID _declspec(naked) HOOK_CHeli__ProcessControl()
{
    _asm
    {
        mov     dwCurrentVehicle, ecx
        pushad
    }

    SwitchContext((CVehicleSAInterface *)dwCurrentVehicle);

    _asm
    {
        popad
        mov     edx, FUNC_CHeli__ProcessControl
        call    edx
        pushad
    }

    ReturnContextToLocalPlayer();
    
    _asm
    {
        popad
        retn
    }
}

DWORD dwCutsceneTrackManagerService = 0x4DBFB0;
DWORD dwAmbienceTrackManagerService = 0x4D76C0;
void _declspec ( naked ) HOOK_CAudioEngine_Service ( )
{
    _asm
    {
        cmp bNotInLocalContext, 1
        je cont
        mov ecx, 0B5F8B8h
        jmp RETURN_CAudioEngine_Service_Cont

cont:
        push edi
        mov ecx, 8AE554h
        call dwCutsceneTrackManagerService
        push edi
        mov ecx, 8AC15Ch
        call dwAmbienceTrackManagerService
        jmp RETURN_CAudioEngine_Service_Fix

    }
}

void _declspec ( naked ) HOOK_CAEVehicleAudioEntity_Service ( )
{
    _asm
    {
        cmp bNotInLocalContext, 1
        je cont
        mov al, [esi+0A5h]
        jmp RETURN_CAEVehicleAudioEntity_Service_Cont

cont:
        jmp RETURN_CAEVehicleAudioEntity_Service_Fix

    }
}
